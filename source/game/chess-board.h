#pragma once

class ChessBoard;

#include <string>
#include <nnpp.hpp>

typedef unsigned char uint8_t;
typedef unsigned int uint32_t;
typedef unsigned long long uint64_t;

static const uint8_t SIZE = 8;
static const uint8_t PAIRS_PER_ROW = 4;
static const uint8_t BITS_PER_TILE = 4;
static const int8_t INVALID = -1;
static const int8_t KING_LONG_CASTLE_X = 2;
static const int8_t KING_SHORT_CASTLE_X = 6;
static const int8_t ROOK_LONG_CASTLE_X = 3;
static const int8_t ROOK_SHORT_CASTLE_X = 5;

struct TileCoords {
	int8_t x : 4;
	int8_t y : 4;

	TileCoords(int8_t x, int8_t y)
			: x(x)
			, y(y) {
		assert(x < SIZE);
		assert(y < SIZE);
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
static_assert(sizeof(TileCoords) == 1);

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
static_assert(sizeof(BoardTilePair) == sizeof(uint8_t));

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
static_assert(sizeof(BoardMove) == 4);

typedef StackVector<BoardMove, 256> MovesVector;

class ChessBoard {
public:
	ChessBoard();
	ChessBoard(const std::string& fen);

	inline bool operator==(const ChessBoard& other) const {
		return m_tileData[0] == other.m_tileData[0]
			&& m_tileData[1] == other.m_tileData[1]
			&& m_tileData[2] == other.m_tileData[2]
			&& m_tileData[3] == other.m_tileData[3]
			&& m_positionData == other.m_positionData;
	}

	inline Color getNextPlayerColor() const {
		return m_positionInfo.nextPlayerColor;
	}

	inline void getNextPlayerMoves(MovesVector& outMoves) const {
		getMoves(m_positionInfo.nextPlayerColor, outMoves);
	}

	inline NNPPStackVector<float> asFloats() const {
		NNPPStackVector<float> res;
		for (uint8_t x = 0; x < 0; ++x) {
			for (uint8_t y = 0; y < 0; ++y) {
				res.push(getTile(x, y).asFloat());
			}
		}
		return res;
	}

	void printBoard() const;
	void printMoveOnBoard(const BoardMove& move) const;
	void getMoves(Color color, MovesVector& outMoves) const;
	void playMove(const BoardMove& move);
	bool isKingInCheck(Color color) const;
	bool isMoveValid(const BoardMove& move) const;
	bool isDraw() const;

private:
	// 4 bits for each tile: 1 bit for color, 3 bits for type
	union {
		uint64_t m_tileData[4];
		BoardTilePair m_boardTiles[32]; // 8 rows of 4 pairs each
	};
	static_assert(sizeof(m_tileData) == sizeof(m_boardTiles));

	// 1 bit for next player, 4 bits for castles, 3 bits for enPassantSquareRow, 3 bits for enPassantSquareCol
	union {
		uint32_t m_positionData;
		struct {
			Color nextPlayerColor : 1;
			bool canWhiteLongCastle : 1;
			bool canBlackLongCastle : 1;
			bool canWhiteShortCastle : 1;
			bool canBlackShortCastle : 1;
			TileCoords enPassantSquare;
			uint32_t pad : 16;
		} m_positionInfo;
	};
	static_assert(sizeof(m_positionInfo) == sizeof(uint32_t));

// Inline Helper functions
	inline uint8_t index(int8_t x, int8_t y) const {
		assert(x < SIZE && y < SIZE);
		return y * PAIRS_PER_ROW + (x >> 1);
	}

	inline bool areCoordsInBoard(const TileCoords& coords) const {
		return coords.x >= 0 && coords.x < SIZE && coords.y >= 0 && coords.y < SIZE;
	}

	inline BoardTile getTile(uint8_t x, uint8_t y) const {
		const bool isFirstPair = (x & 0b1) == 0;
		const BoardTilePair& pair = m_boardTiles[index(x, y)];
		return isFirstPair ? BoardTile(pair.color0, pair.type0) : BoardTile(pair.color1, pair.type1);
	}

	inline BoardTile getTile(const TileCoords& coords) const { 
		return getTile(coords.x, coords.y);
	}

	inline void setTile(uint8_t x, uint8_t y, const BoardTile& tile) {
		const bool isFirstPair = (x & 0b1) == 0;
		BoardTilePair& pair = m_boardTiles[index(x, y)];
		if (isFirstPair) {
			pair.color0 = tile.color;
			pair.type0 = tile.type;
		}
		else {
			pair.color1 = tile.color;
			pair.type1 = tile.type;
		}
	}

	inline void setTile(const TileCoords& coords, const BoardTile& tile) {
		setTile(coords.x, coords.y, tile);
	}

	inline void removePiece(uint8_t x, uint8_t y) {
		assert(getTile(x, y).type != EMPTY);
		setTile(x, y, BoardTile());
	}

	inline void removePiece(const TileCoords& coords) {
		removePiece(coords.x, coords.y);
	}

// Helper functions
	TileCoords findKing(Color color) const;
	void getMovesForPiece(BoardTile tile, uint8_t x, uint8_t y, MovesVector& outMoves) const;
	void getDirectionalMoves(Color color, int8_t sx, int8_t sy, int8_t dx, int8_t dy, MovesVector& outMoves) const;
	void getSurroundingMoves(Color color, int8_t sx, int8_t sy, MovesVector& outMoves) const;
	void getBishopMoves(Color color, int8_t sx, int8_t sy, MovesVector& outMoves) const;
	void getRookMoves(Color color, int8_t sx, int8_t sy, MovesVector& outMoves) const;
	void getKnightMoves(Color color, int8_t sx, int8_t sy, MovesVector& outMoves) const;
	void getQueenMoves(Color color, int8_t sx, int8_t sy, MovesVector& outMoves) const;
	void getPawnMoves(Color color, int8_t sx, int8_t sy, MovesVector& outMoves) const;
	void getKingMoves(Color color, int8_t sx, int8_t sy, MovesVector& outMoves) const;
};
