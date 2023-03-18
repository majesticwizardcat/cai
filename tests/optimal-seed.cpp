#include "game/chess-board.h"
#include "tools/board-hashing.hpp"

#include "unordered_dense.h"

#include <iostream>
#include <ios>

// Generates the most optimal hash table possible
const uint64_t POSITIONS_TO_TEST = 1000000000000;
const boardhashing::pcg32_random_t LAST_POSITION_CHECKED = { 0x66664e33e91bc3d0, 0x0 };

// Copied from board hashing
typedef std::array<uint64_t, 1024> HashTable;

static constexpr uint32_t TABLES_NEEDED_FOR_INFO = 1; // castles, next player move
static constexpr uint32_t TABLES_NEEDED_FOR_EN_PASSANT = 1;
static constexpr uint32_t TABLES_NEEDED_FOR_BOARD = 2 * 7;
static constexpr uint32_t TABLES_NEEDED = TABLES_NEEDED_FOR_INFO + TABLES_NEEDED_FOR_EN_PASSANT + TABLES_NEEDED_FOR_BOARD;
static constexpr uint32_t TABLE_SIZE = BOARD_SIZE * BOARD_SIZE;
static constexpr uint32_t HASH_TABLE_SIZE = TABLE_SIZE * TABLES_NEEDED; // = 1024 (2 ^ 10)

HashTable createHashTable(boardhashing::pcg32_random_t* rgen) {
	HashTable table;
	for (uint64_t i = 0; i < table.size(); ++i) {
		table[i] = boardhashing::pcg32_random_r(rgen);
		table[i] = (table[i] << 32) | boardhashing::pcg32_random_r(rgen);
	}
	return table;
}

inline uint64_t getNextPlayerColorHashValue(const HashTable& hashTable, Color color) {
	return hashTable[static_cast<uint32_t>(color)];
}

inline uint64_t getCastleHashValue(const HashTable& hashTable, bool isCastleAvailable, Color color, bool isLongCastle) {
	const uint32_t longCastlePad = static_cast<uint32_t>(isLongCastle);
	const uint32_t castlePad = static_cast<uint32_t>(isCastleAvailable) << 1;
	const uint32_t colorPad = static_cast<uint32_t>(color) << 2;
	const uint32_t index = (colorPad | castlePad | longCastlePad) + 2;
	assert(index >= 2 && index < 8 + 2);
	return hashTable[index];
}

inline uint64_t getEnPassantHashValue(const HashTable& hashTable, const TileCoords& coords) {
	if (!coords.areValid()) {
		return 0;
	}

	const uint32_t infoPad = TABLES_NEEDED_FOR_INFO;
	const uint32_t coordsPad = coords.y * BOARD_SIZE + coords.x;
	const uint32_t index = infoPad * TABLE_SIZE + coordsPad;
	assert(index >= TABLE_SIZE * TABLES_NEEDED_FOR_INFO
		&& index < TABLE_SIZE * (TABLES_NEEDED_FOR_INFO + TABLES_NEEDED_FOR_EN_PASSANT));
	return hashTable[index];
}

inline uint64_t getBoardHashValue(const HashTable& hashTable, const BoardTile& tile, const TileCoords& coords) {
	assert(coords.areValid());
	const uint32_t infoAndEnPassantPad = TABLES_NEEDED_FOR_INFO + TABLES_NEEDED_FOR_EN_PASSANT;
	const uint32_t colorPad = static_cast<uint32_t>(tile.color) * 7;
	const uint32_t coordsPad = coords.y * BOARD_SIZE + coords.x;
	const uint32_t index = (infoAndEnPassantPad + tile.type + colorPad) * TABLE_SIZE + coordsPad;
	assert(tile.type != EMPTY || tile.type + colorPad == 0);
	assert(index >= TABLE_SIZE * (TABLES_NEEDED_FOR_INFO + TABLES_NEEDED_FOR_EN_PASSANT)
		&& index < TABLE_SIZE * TABLES_NEEDED);
	return hashTable[index];
}

uint64_t getHashFromBoard(const ChessBoard& board, const HashTable& hashTable) {
	uint64_t hash = 0;
	hash ^= getNextPlayerColorHashValue(hashTable, board.getNextPlayerColor());
	hash ^= getCastleHashValue(hashTable, board.canWhiteLongCastle(), WHITE, true);
	hash ^= getCastleHashValue(hashTable, board.canWhiteShortCastle(), WHITE, false);
	hash ^= getCastleHashValue(hashTable, board.canBlackLongCastle(), BLACK, true);
	hash ^= getCastleHashValue(hashTable, board.canBlackShortCastle(), BLACK, false);
	hash ^= getEnPassantHashValue(hashTable, board.getEnPassantCoords());

	for (uint8_t x = 0; x < BOARD_SIZE; ++x) {
		for (uint8_t y = 0; y < BOARD_SIZE; ++y) {
			TileCoords coords(x, y);
			hash ^= getBoardHashValue(hashTable, board.getTile(coords), coords);
		}
	}

	return hash;
}

uint64_t getCollisions(boardhashing::pcg32_random_t seed) {
	ankerl::unordered_dense::map<uint64_t, ChessBoard> positions;
	ChessBoard board;
	HashTable table = createHashTable(&seed);

	boardhashing::pcg32_random_t moveGen = { 0x66664e33e91bc3d0, 0x54fcccaf93f7dc31 };
	uint64_t collisions = 0;

	MovesVector moves;
	for (uint64_t k = 0; k < POSITIONS_TO_TEST; ++k) {
		uint64_t hash = getHashFromBoard(board, table);
		auto it = positions.find(hash);
		if (it != positions.end() && it->second != board) {
			collisions++;
		}
		else if (it == positions.end()) {
			positions[hash] = board;
		}

		moves.clear();
		board.getNextPlayerMoves(moves);
		if (moves.empty() || board.isDraw()) {
			board = ChessBoard();
		}
		else {
			board.playMove(moves[boardhashing::pcg32_random_r(&moveGen) % moves.size()]);
		}
	}
	return collisions;
}

int main(int args, char** argv) {
//	assert(false); // Only run in debug if required
	std::cout << "Looking for optimal seed starting with seed gen with state: 0x" << std::hex << LAST_POSITION_CHECKED.state << ", incr: 0x" << LAST_POSITION_CHECKED.inc << '\n';
	std::cout << "Currently trying " << std::dec << POSITIONS_TO_TEST << " positions per seed" << '\n';
	std::cout << "This might take a while..." << '\n';
	boardhashing::pcg32_random_t seedGenerator = LAST_POSITION_CHECKED;
	boardhashing::pcg32_random_t optimalSeed = { boardhashing::INITIAL_SEED, boardhashing::INITIAL_INCR }; // starting with current optimal that we use
	uint64_t optimalSeedCollisions = getCollisions(optimalSeed);
	std::cout << "Starting optimal: Seed: 0x" << std::hex << optimalSeed.state << ", Incr: 0x" << optimalSeed.inc << ", collisions: " << std::dec << optimalSeedCollisions << '\n';

	// We just run forever until we stop it or we find a seed with no collisions, we store the last result to continue for the next time
	// If we find a better one we save it. If we find a one with 0 collisions, we increase the positions to check for the next time
	// We print the new optimal one evey time we find a new one
	std::mutex seedGenMutex;
	std::mutex optimalSeedMutex;
	auto threadWorker = [&]() {
		while (optimalSeedCollisions > 0) {
			boardhashing::pcg32_random_t gen;
			boardhashing::pcg32_random_t current;

			{
				std::lock_guard<std::mutex> lock(seedGenMutex);
				gen = seedGenerator;
				current.state = boardhashing::pcg32_random_r(&seedGenerator);
				current.state = (current.state << 32) | boardhashing::pcg32_random_r(&seedGenerator);

				current.inc = boardhashing::pcg32_random_r(&seedGenerator); 
				current.inc = (current.inc << 32) | boardhashing::pcg32_random_r(&seedGenerator);
			}

			uint64_t collisions = getCollisions(current);

			{
				std::lock_guard<std::mutex> lock(optimalSeedMutex);
				std::cout << "Checked: Seed: 0x" << std::hex << current.state << ", Incr: 0x" << current.inc << ", collisions: " << std::dec << collisions
					<< std::hex << " || Generator: Seed: 0x" << gen.state << ", Incr: 0x" << gen.inc << std::dec <<  '\n';

				if (collisions < optimalSeedCollisions) {
					optimalSeed = current;
					optimalSeedCollisions = collisions;
					std::cout << "Current optimal -> Seed: 0x"
						<< std::hex << optimalSeed.state << ", Incr: 0x" << optimalSeed.inc << ", collisions: " << std::dec << optimalSeedCollisions << '\n';
				}
			}
		}
	};

	int threadCount = 1;
	if (args > 1) {
		threadCount = atoi(argv[1]);
	}

	std::vector<std::thread> threads;
	for (int i = 0; i < threadCount; ++i) {
		threads.emplace_back(threadWorker);
	}

	for (auto& t : threads) {
		t.join();
	}

	std::cout << "Stopped because a no collision seed was found with State: "
		<< std::hex << optimalSeed.state << ", Incr: " << optimalSeed.inc << ", collisions: " << std::dec << optimalSeedCollisions << '\n';
}
