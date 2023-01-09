#pragma once

class ChessBoard;

#include "game/chess-board-structs.hpp"

#include <string>

static const uint8_t PAIRS_PER_ROW = 4;
static const uint8_t BITS_PER_TILE = 4;
static const int8_t KING_LONG_CASTLE_X = 2;
static const int8_t KING_SHORT_CASTLE_X = 6;
static const int8_t ROOK_LONG_CASTLE_X = 3;
static const int8_t ROOK_SHORT_CASTLE_X = 5;
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

// Inline Helper functions
	inline uint8_t index(int8_t x, int8_t y) const {
		assert(x < BOARD_SIZE && y < BOARD_SIZE);
		return y * PAIRS_PER_ROW + (x >> 1);
	}

	inline bool areCoordsInBoard(const TileCoords& coords) const {
		return coords.x >= 0 && coords.x < BOARD_SIZE && coords.y >= 0 && coords.y < BOARD_SIZE;
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
