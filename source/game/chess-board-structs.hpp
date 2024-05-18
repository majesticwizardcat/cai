#pragma once

#include <cassert>
#include "nnpp.hpp"

static constexpr uint8_t BOARD_SIZE = 8;
static constexpr int8_t INVALID = -1;
static constexpr uint64_t MAX_MOVES = 255;

struct TileCoords {
	int8_t x : 4;
	int8_t y : 4;

	constexpr TileCoords(const int8_t x, const int8_t y)
			: x(x)
			, y(y) {
		assert(x < BOARD_SIZE);
		assert(y < BOARD_SIZE);
	}

	TileCoords() = default;

	inline constexpr bool operator==(const TileCoords other) const {
		return x == other.x && y == other.y;
	}

	inline constexpr bool areValid() const {
		assert((x != INVALID && y != INVALID) || (x == INVALID && y == INVALID));
		return x != INVALID; // if one's invalid, both should
	}

	inline constexpr bool areOutsideBoard() const {
		return x < 0 || y < 0; // board ranges at [0, 7]. Each coords ranges from [-7, 7]
	}
};

enum Color : uint8_t {
	WHITE,
	BLACK,
};

enum TileType : uint8_t {
	EMPTY,
	PAWN,
	KNIGHT,
	BISHOP,
	ROOK,
	QUEEN,
	KING,
	NUM_OF_TYPES,
};

struct BoardTilePair {
	Color color0 : 1;
	TileType type0 : 3;
	Color color1 : 1;
	TileType type1 : 3;
};

struct BoardTile {
	union {
		struct {
			Color color : 1;
			TileType type : 3;
		};
		uint8_t data;
	};

	BoardTile() = default;

	BoardTile(const Color color, const TileType type)
			: color(color)
			, type(type) { }

	constexpr BoardTile(const uint8_t fromUint8)
			: data(fromUint8) {}

	inline constexpr operator char() const {
		switch(type) {
		case TileType::PAWN:
			return color == Color::WHITE ? 'P' : 'p';
		case TileType::KNIGHT:
			return color == Color::WHITE ? 'N' : 'n';
		case TileType::BISHOP:
			return color == Color::WHITE ? 'B' : 'b';
		case TileType::ROOK:
			return color == Color::WHITE ? 'R' : 'r';
		case TileType::QUEEN:
			return color == Color::WHITE ? 'Q' : 'q';
		case TileType::KING:
			return color == Color::WHITE ? 'K' : 'k';
		default:
			return ' ';
		};
		return ' ';
	}

	inline constexpr float asFloat() const {
		float c = color == WHITE ? 1.0f : -1.0f;
		switch(type) {
		case PAWN:
			return 1.0f * c;
		case KNIGHT:
			return 3.0f * c;
		case BISHOP:
			return 3.5f * c;
		case ROOK:
			return 5.0f * c;
		case QUEEN:
			return 9.0f * c;
		case KING:
			return 10.0f * c;
		default:
			return 0.0f;
		};
		return 0.0f;
	}
};

struct BoardMove {
	TileCoords from;
	TileCoords to;
	TileCoords enPassantPawn;
	TileType promotionType;

	BoardMove() = default;

	constexpr BoardMove(const uint8_t fromX, const uint8_t fromY, const uint8_t toX, const uint8_t toY)
			: promotionType(EMPTY)
			, enPassantPawn(INVALID, INVALID)
			, from(fromX, fromY)
			, to(toX, toY) { }
	
	constexpr BoardMove(const TileCoords from, const TileCoords to)
			: promotionType(EMPTY)
			, enPassantPawn(INVALID, INVALID)
			, from(from)
			, to(to) { }

	inline constexpr bool isCastle(const TileType type) const { 
		assert(from.areValid() && to.areValid());
		return type == KING && std::abs(from.x - to.x) == 2;
	}
};

typedef sauce::StaticVector<BoardMove, MAX_MOVES> MovesVector;
