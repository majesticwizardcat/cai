#pragma once

#include "game/chess-board-structs.hpp"

#include <array>

namespace boardhashing {

// *Really* minimal PCG32 code / (c) 2014 M.E. O'Neill / pcg-random.org
// Licensed under Apache License 2.0 (NO WARRANTY, etc. see website)

struct pcg32_random_t {
	uint64_t state;
	uint64_t inc;

	inline bool operator==(const pcg32_random_t& other) const {
		return memcmp(this, &other, sizeof(pcg32_random_t)) == 0;
	}
};

static constexpr uint32_t pcg32_random_r(pcg32_random_t* rng) {
    uint64_t oldstate = rng->state;
    // Advance internal state
    rng->state = oldstate * 6364136223846793005ULL + (rng->inc++|1);
    // Calculate output function (XSH RR), uses old state for max ILP
    uint32_t xorshifted = ((oldstate >> 18u) ^ oldstate) >> 27u;
    uint32_t rot = oldstate >> 59u;
    return (xorshifted >> rot) | (xorshifted << ((-rot) & 31));
}

// These values are generated at tests/optimal-seed.cpp
static constexpr uint64_t INITIAL_SEED = 0x7ebd10b0ce1ba8c9;
static constexpr uint64_t INITIAL_INCR = 0x297a276938712deb;

class BoardHashTable {
public:
	constexpr BoardHashTable()
			: m_hashTable(createHashTable()) {
	}

	constexpr uint64_t getNextPlayerColorHashValue(const Color color) const {
		return m_hashTable[static_cast<uint32_t>(color)];
	}

	constexpr uint64_t getCastleHashValue(const bool isCastleAvailable, const Color color, const bool isLongCastle) const {
		const uint32_t longCastlePad = static_cast<uint32_t>(isLongCastle); // 2 per castle type (long at 0, short at 1)
		const uint32_t castlePad = static_cast<uint32_t>(isCastleAvailable) << 1; // 2 pairs (4) per availability (2 for available, 2 for not)
		const uint32_t colorPad = static_cast<uint32_t>(color) << 2;
		const uint32_t index = (colorPad | castlePad | longCastlePad) + 2; // pading for 0 and 1 because these are for color hashes
		assert(index >= 2 && index < 8 + 2);
		return m_hashTable[index];
	}

	constexpr uint64_t getEnPassantHashValue(const TileCoords coords) const {
		assert(coords.areValid());
		constexpr uint32_t infoPad = TABLES_NEEDED_FOR_INFO;
		const uint32_t coordsPad = coords.y * BOARD_SIZE + coords.x;
		const uint32_t index = infoPad * TABLE_SIZE + coordsPad;
		assert(index >= TABLE_SIZE * TABLES_NEEDED_FOR_INFO
			&& index < TABLE_SIZE * (TABLES_NEEDED_FOR_INFO + TABLES_NEEDED_FOR_EN_PASSANT));
		return m_hashTable[index];
	}

	constexpr uint64_t getBoardHashValue(const BoardTile tile, const TileCoords coords) const {
		// if type is empty we start at 0
		// else we move 1 to 12 more positions
		// plus the extra padding we need from the 2 boards that hold info
		assert(coords.areValid());
		constexpr uint32_t infoAndEnPassantPad = TABLES_NEEDED_FOR_INFO + TABLES_NEEDED_FOR_EN_PASSANT; // 2 boards for info and en passant
		const uint32_t colorPad = static_cast<uint32_t>(tile.color) * 7;
		const uint32_t coordsPad = coords.y * BOARD_SIZE + coords.x;
		const uint32_t index = (infoAndEnPassantPad + tile.type + colorPad) * TABLE_SIZE + coordsPad;
		assert(tile.type != EMPTY || tile.type + colorPad == 0); // always must be 0, otherwise a removal of piece didn't set the color to WHITE
		assert(index >= TABLE_SIZE * (TABLES_NEEDED_FOR_INFO + TABLES_NEEDED_FOR_EN_PASSANT)
			&& index < TABLE_SIZE * TABLES_NEEDED);
		return m_hashTable[index];
	}

private:
	// We need 64 hashes for every location on the board for every piece type (6 types) and the colors (2 colors) => 12 * 64
	// +2 * 64 for an empty tile => 14 * 64 (we only use one of these)
	// +1 * 64 for each possible en passant tile => 15 * 64
	// +8 values for castling (2 types of castling, 2 colors, 2 states each)
	// +2 for next player (colors)
	/// total of 16 boards * 64 => 1024
	static constexpr uint32_t TABLES_NEEDED_FOR_INFO = 1; // castles, next player move
	static constexpr uint32_t TABLES_NEEDED_FOR_EN_PASSANT = 1;
	static constexpr uint32_t TABLES_NEEDED_FOR_BOARD = 2 * 7;
	static constexpr uint32_t TABLES_NEEDED = TABLES_NEEDED_FOR_INFO + TABLES_NEEDED_FOR_EN_PASSANT + TABLES_NEEDED_FOR_BOARD;
	static constexpr uint32_t TABLE_SIZE = BOARD_SIZE * BOARD_SIZE;
	static constexpr uint32_t HASH_TABLE_SIZE = TABLE_SIZE * TABLES_NEEDED; // = 1024 (2 ^ 10)
	static_assert(TABLES_NEEDED == 16);
	static_assert(HASH_TABLE_SIZE == 1024);

	typedef std::array<uint64_t, HASH_TABLE_SIZE> HashTableArray;
	HashTableArray m_hashTable;

	constexpr HashTableArray createHashTable() {
		pcg32_random_t pcgState = { INITIAL_SEED, INITIAL_INCR };

		HashTableArray array = { };
		for (uint32_t i = 0; i < HASH_TABLE_SIZE; ++i) {
			array[i] = pcg32_random_r(&pcgState);
			array[i] = (array[i] << 32) | pcg32_random_r(&pcgState);
		}
		return array;
	}
};

static constexpr BoardHashTable BOARD_HASH_TABLE = { };

}
