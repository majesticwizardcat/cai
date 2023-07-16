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
		return memcmp(this, &other, sizeof(ChessBoard)) == 0;
	}

	inline bool operator!=(const ChessBoard& other) const {
		return memcmp(this, &other, sizeof(ChessBoard)) != 0;
	}

	inline size_t getHash() const {
		return m_hash;
	}

	inline Color getNextPlayerColor() const {
		return m_positionInfo.nextPlayerColor;
	}

	inline void getNextPlayerMoves(MovesVector& outMoves) const {
		getMoves(m_positionInfo.nextPlayerColor, outMoves);
	}

	inline NNPPStackVector<float> asFloats() const {
		NNPPStackVector<float> res;
		for (uint8_t x = 0; x < 8; ++x) {
			for (uint8_t y = 0; y < 8; ++y) {
				res.push(getTile(x, y).asFloat());
			}
		}
		return res;
	}

	inline BoardTile getTile(uint8_t x, uint8_t y) const {
		const bool isFirstPair = (x & 0b1) == 0;
		const BoardTilePair& pair = m_boardTiles[index(x, y)];
		return isFirstPair ? BoardTile(pair.color0, pair.type0) : BoardTile(pair.color1, pair.type1);
	}

	inline BoardTile getTile(const TileCoords& coords) const { 
		return getTile(coords.x, coords.y);
	}

	inline bool canWhiteLongCastle() const {
		return m_positionInfo.canWhiteLongCastle;
	}

	inline bool canBlackLongCastle() const {
		return m_positionInfo.canBlackLongCastle;
	}

	inline bool canWhiteShortCastle() const {
		return m_positionInfo.canWhiteShortCastle;
	}

	inline bool canBlackShortCastle() const {
		return m_positionInfo.canBlackShortCastle;
	}

	inline TileCoords getEnPassantCoords() const {
		return m_positionInfo.enPassantSquare;
	}

	inline bool isKingInCheck(Color color) const {
		TileCoords kingCoords = findKing(color);
		return isAttacked(color, kingCoords);
	}

	void printBoard() const;
	void printMoveOnBoard(const BoardMove& move) const;
	void getMoves(Color color, MovesVector& outMoves) const;
	void playMove(const BoardMove& move);
	void calculateHashFromCurrentState();
	bool isAttacked(Color color, const TileCoords& coords) const;
	bool isMoveValid(const BoardMove& move, const TileCoords& kingCoords) const;
	bool isDraw() const;

private:
	// 4 bits for each tile: 1 bit for color, 3 bits for type
	union {
		uint64_t m_tileData[4];
		BoardTilePair m_boardTiles[32]; // 8 rows of 4 pairs each
	};

	// 1 bit for next player, 4 bits for castles, 8 bits for en passant square
	union {
		uint16_t m_positionData;
		struct {
			Color nextPlayerColor : 1;
			bool canWhiteLongCastle : 1;
			bool canBlackLongCastle : 1;
			bool canWhiteShortCastle : 1;
			bool canBlackShortCastle : 1;
			TileCoords enPassantSquare;
		} m_positionInfo;
	};

	uint64_t m_hash;

// Inline Helper functions
	inline uint8_t index(int8_t x, int8_t y) const {
		assert(x < BOARD_SIZE && y < BOARD_SIZE);
		return y * PAIRS_PER_ROW + (x >> 1);
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
	void updateBoardDataFromMove(const BoardMove& move, BoardTile fromTile);
};

struct BoardHasher {
	inline size_t operator()(const ChessBoard& board) const {
		return board.getHash();
	}
};
	