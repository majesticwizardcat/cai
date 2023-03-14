#pragma once

#include "game/chess-board-structs.hpp"

#include <array>

namespace boardhashing {

typedef unsigned long long uint64_t;
typedef unsigned int uint32_t;

// *Really* minimal PCG32 code / (c) 2014 M.E. O'Neill / pcg-random.org
// Licensed under Apache License 2.0 (NO WARRANTY, etc. see website)

typedef struct { uint64_t state;  uint64_t inc; } pcg32_random_t;

static constexpr uint32_t pcg32_random_r(pcg32_random_t* rng) {
    uint64_t oldstate = rng->state;
    // Advance internal state
    rng->state = oldstate * 6364136223846793005ULL + (rng->inc|1);
    // Calculate output function (XSH RR), uses old state for max ILP
    uint32_t xorshifted = ((oldstate >> 18u) ^ oldstate) >> 27u;
    uint32_t rot = oldstate >> 59u;
    return (xorshifted >> rot) | (xorshifted << ((-rot) & 31));
}

class BoardHashTable {
public:
	constexpr BoardHashTable()
			: m_hashTable(createHashTable()) {
	}

	inline uint64_t getNextPlayerColorHashValue(Color color) const {
		const uint32_t index = color == WHITE ? 0 : 1;
		return m_hashTable[index];
	}

	inline uint64_t getCastleHashValue(bool isCastleAvailable, Color color, bool isLongCastle) const {
		const uint32_t nextPlayerPad = 2;
		const uint32_t longCastlePad = isLongCastle ? 0 : 1; // 2 per castle type (long at 0, short at 1)
		const uint32_t castlePad = isCastleAvailable ? 0 : 2; // 2 pairs (4) per availability (2 for available, 2 for not)
		const uint32_t colorPad = color == WHITE ? 0 : 4; // 4 per color
		const uint32_t index = nextPlayerPad + colorPad + castlePad + longCastlePad;
		assert(index >= 1 && index < 8 + 2);
		return m_hashTable[index];
	}

	inline uint64_t getEnPassantHashValue(const TileCoords& coords) const {
		assert(coords.areValid());
		const uint32_t infoPad = TABLES_NEEDED_FOR_INFO;
		const uint32_t coordsPad = coords.y * BOARD_SIZE + coords.x;
		const uint32_t index = infoPad * TABLE_SIZE + coordsPad;
		assert(index >= TABLE_SIZE * TABLES_NEEDED_FOR_INFO
			&& index < TABLE_SIZE * (TABLES_NEEDED_FOR_INFO + TABLES_NEEDED_FOR_EN_PASSANT));
		return m_hashTable[index];
	}

	inline uint64_t getBoardHashValue(const BoardTile& tile, const TileCoords& coords) const {
		assert(coords.areValid());
		const uint32_t infoAndEnPassantPad = TABLES_NEEDED_FOR_INFO + TABLES_NEEDED_FOR_EN_PASSANT;
		const uint32_t typePad = tile.type;
		const uint32_t colorPad = tile.color == typePad == 0 ? 0 : (tile.color == WHITE ? 1 : NUM_OF_TYPES);
		const uint32_t coordsPad = coords.y * BOARD_SIZE + coords.x;
		const uint32_t index = (infoAndEnPassantPad + typePad + colorPad) * TABLE_SIZE + coordsPad;
		assert(index >= TABLE_SIZE * (TABLES_NEEDED_FOR_INFO + TABLES_NEEDED_FOR_EN_PASSANT)
			&& index < TABLE_SIZE * TABLES_NEEDED);
		return m_hashTable[index];
	}

private:
	// We need 64 hashes for every location on the board for every piece type (6 types) and the colors (2 colors) => 12 * 64
	// +1 * 64 for an empty tile => 13 * 64
	// +1 * 64 for each possible en passant tile => 14 * 64
	// +8 values for castling (2 types of castling, 2 colors, 2 states each)
	// +2 for next player (colors)
	// + some padding to make it up to 2 ^ 10 (maybe the space will be used in the future for other inputs)
	static constexpr uint32_t TABLES_NEEDED_FOR_INFO = 1; // castles, next player move
	static constexpr uint32_t TABLES_NEEDED_FOR_EN_PASSANT = 1;
	static constexpr uint32_t TABLES_NEEDED_FOR_BOARD = NUM_OF_COLORS * NUM_OF_TYPES - 1; // -1 because empty tiles are not needed per color
	static constexpr uint32_t TABLES_NEEDED = TABLES_NEEDED_FOR_INFO + TABLES_NEEDED_FOR_EN_PASSANT + TABLES_NEEDED_FOR_BOARD;
	static constexpr uint32_t TABLES_NEEDED_FOR_1024_PADDING = 16 - TABLES_NEEDED;
	static constexpr uint32_t TABLE_SIZE = BOARD_SIZE * BOARD_SIZE;
	static constexpr uint32_t HASH_TABLE_SIZE = TABLE_SIZE * (TABLES_NEEDED + TABLES_NEEDED_FOR_1024_PADDING); // = 1024 (2 ^ 10)
	static_assert(TABLES_NEEDED + TABLES_NEEDED_FOR_1024_PADDING == 16);
	static_assert(HASH_TABLE_SIZE == 1024);
	//	Guess the sha256 value(first 16 hex digits): 3 words, 10 small letters, words are concated together (likethisforexample)
	static constexpr uint64_t INITIAL_SEED = 0x66664e33e91bc3d0;
	static constexpr uint64_t INITIAL_INCR = 0xfe2c8a1959ac4550; // this ones is 3 words, 12 small letters

	typedef std::array<uint64_t, HASH_TABLE_SIZE> HashTableArray;
	HashTableArray m_hashTable;

	constexpr HashTableArray createHashTable() {
		pcg32_random_t pcgState = { INITIAL_SEED, INITIAL_INCR };

		HashTableArray array = { };
		for (uint32_t i = 0; i < HASH_TABLE_SIZE; ++i) {
			array[i] = pcg32_random_r(&pcgState);
			array[i] = (array[i] << 32) + pcg32_random_r(&pcgState);
		}
		return array;
	}
};

static constexpr BoardHashTable BOARD_HASH_TABLE = { };

}
