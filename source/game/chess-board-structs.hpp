#pragma once

#include <nnpp.hpp>

typedef unsigned char uint8_t;
typedef unsigned int uint32_t;
typedef unsigned long long uint64_t;

static const uint8_t BOARD_SIZE = 8;
static const int8_t INVALID = -1;

struct TileCoords {
	int8_t x : 4;
	int8_t y : 4;

	TileCoords(int8_t x, int8_t y)
			: x(x)
			, y(y) {
		assert(x < BOARD_SIZE);
		assert(y < BOARD_SIZE);
	}

	TileCoords() : TileCoords(INVALID, INVALID) { }

	inline bool operator==(const TileCoords& other) const {
		return x == other.x && y == other.y;
	}

	inline bool areValid() const {
		assert((x != INVALID && y != INVALID) || (x == INVALID && y == INVALID));
		return x != INVALID;
	}
};

enum Color : uint8_t {
	WHITE,
	BLACK,
	NUM_OF_COLORS,
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
	Color color : 1;
	TileType type : 3;

	BoardTile() : BoardTile(WHITE, EMPTY) { }

	BoardTile(Color color, TileType type)
			: color(color)
			, type(type) { }

	BoardTile(uint8_t fromUint8) {
		constexpr uint8_t colorMask = 0b1;
		constexpr uint8_t typeMask = 0b111;
		color = static_cast<Color>(fromUint8 & colorMask);
		type = static_cast<TileType>((fromUint8 >> 1) & typeMask);
	}

	inline operator uint8_t() const {
		return color | (type << 1);
	}

	inline operator char() const {
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

	inline float asFloat() const {
		float color = color == WHITE ? 1.0f : -1.0f;
		float start = 0.0f;
		switch(type) {
		case PAWN:
			return 1.0f * color;
		case KNIGHT:
			return 3.0f * color;
		case BISHOP:
			return 3.5f * color;
		case ROOK:
			return 5.0f * color;
		case QUEEN:
			return 9.0f * color;
		case KING:
			return 10.0f * color;
		default:
			return 0.0f;
		};
		return 0.0f;
	}
};

struct BoardMove {
	// The rook coords are not needed and will be removed, reducing the BoardMove to a size of 4 bytes;
	TileCoords from;
	TileCoords to;
	TileCoords enPassantPawn;
	TileType promotionType;

	BoardMove()
			: promotionType(EMPTY) { }

	BoardMove(uint8_t fromX, uint8_t fromY, uint8_t toX, uint8_t toY)
			: promotionType(EMPTY)
			, from(fromX, fromY)
			, to(toX, toY) { }
	
	BoardMove(const TileCoords& from, const TileCoords& to)
			: promotionType(EMPTY)
			, from(from)
			, to(to) { }
	
	inline bool isCastle(TileType type) const { 
		assert(from.areValid() && to.areValid());
		return type == KING && std::abs(from.x - to.x) == 2;
	}
};
typedef StackVector<BoardMove, 256> MovesVector;
