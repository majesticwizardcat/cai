#pragma once

class ChessBoard;

#include "game/chess-board-structs.hpp"

#include <string>

static constexpr uint8_t PAIRS_PER_ROW = 4;
static constexpr uint8_t BITS_PER_TILE = 4;
static constexpr int8_t KING_LONG_CASTLE_X = 2;
static constexpr int8_t KING_SHORT_CASTLE_X = 6;
static constexpr int8_t ROOK_LONG_CASTLE_X = 3;
static constexpr int8_t ROOK_SHORT_CASTLE_X = 5;

class ChessBoard {
public:
	ChessBoard();
	ChessBoard(const std::string& fen);

	inline constexpr bool operator==(const ChessBoard& other) const {
		return m_hash == other.m_hash
			&& m_positionData == other.m_positionData
			&& m_tileData[0] == other.m_tileData[0]
			&& m_tileData[1] == other.m_tileData[1]
			&& m_tileData[2] == other.m_tileData[2]
			&& m_tileData[3] == other.m_tileData[3];
	}

	inline constexpr bool operator!=(const ChessBoard& other) const {
		return m_hash != other.m_hash
			|| m_positionData != other.m_positionData
			|| m_tileData[0] != other.m_tileData[0]
			|| m_tileData[1] != other.m_tileData[1]
			|| m_tileData[2] != other.m_tileData[2]
			|| m_tileData[3] != other.m_tileData[3];
	}

	inline constexpr uint64_t getHash() const {
		return m_hash;
	}

	inline constexpr Color getNextPlayerColor() const {
		return m_positionInfo.nextPlayerColor;
	}

	inline constexpr void getNextPlayerMoves(MovesVector& outMoves) const {
		getMoves(m_positionInfo.nextPlayerColor, outMoves);
	}

	inline constexpr nnpp::NNPPStackVector<float> asFloats() const {
		nnpp::NNPPStackVector<float> res;
		for (uint8_t x = 0; x < 8; ++x) {
			for (uint8_t y = 0; y < 8; ++y) {
				res.push(getTile(x, y).asFloat());
			}
		}
		return res;
	}

	inline BoardTile getTile(const uint8_t x, const uint8_t y) const {
		const bool isFirstPair = (x & 0b1) == 0;
		const BoardTilePair pair = m_boardTiles[index(x, y)];
		return isFirstPair ? BoardTile(pair.color0, pair.type0) : BoardTile(pair.color1, pair.type1);
	}

	inline constexpr BoardTile getTile(const TileCoords coords) const { 
		return getTile(coords.x, coords.y);
	}

	inline constexpr bool canWhiteLongCastle() const {
		return m_positionInfo.canWhiteLongCastle;
	}

	inline constexpr bool canBlackLongCastle() const {
		return m_positionInfo.canBlackLongCastle;
	}

	inline constexpr bool canWhiteShortCastle() const {
		return m_positionInfo.canWhiteShortCastle;
	}

	inline constexpr bool canBlackShortCastle() const {
		return m_positionInfo.canBlackShortCastle;
	}

	inline constexpr TileCoords getEnPassantCoords() const {
		return m_positionInfo.enPassantSquare;
	}

	inline constexpr bool isKingInCheck(const Color color) const {
		const TileCoords kingCoords = findKing(color);
		return isAttacked(color, kingCoords);
	}

	void printBoard() const;
	void printMoveOnBoard(const BoardMove move) const;
	void calculateHashFromCurrentState();

	void getMoves(const Color color, MovesVector& outMoves) const;
	void playMove(const BoardMove move);
	bool isAttacked(const Color color, const TileCoords coords) const;
	bool isMoveValid(const BoardMove move, const TileCoords kingCoords) const;
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

	inline constexpr uint8_t index(const int8_t x, const int8_t y) const {
		assert(x < BOARD_SIZE && y < BOARD_SIZE);
		return y * PAIRS_PER_ROW + (x >> 1);
	}

	inline constexpr void setTile(const uint8_t x, const uint8_t y, const BoardTile tile) {
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

	inline constexpr void setTile(const TileCoords coords, const BoardTile tile) {
		setTile(coords.x, coords.y, tile);
	}

	inline constexpr void removePiece(const uint8_t x, const uint8_t y) {
		assert(getTile(x, y).type != EMPTY);
		setTile(x, y, BoardTile(0));
	}

	inline constexpr void removePiece(const TileCoords coords) {
		removePiece(coords.x, coords.y);
	}

	TileCoords findKing(const Color color) const;
	void getMovesForPiece(const BoardTile tile, const uint8_t x, const uint8_t y, MovesVector& outMoves) const;
	void getDirectionalMoves(const Color color, const int8_t sx, const int8_t sy, const int8_t dx, const int8_t dy, MovesVector& outMoves) const;
	void getSurroundingMoves(const Color color, const int8_t sx, const int8_t sy, MovesVector& outMoves) const;
	void getBishopMoves(const Color color, const int8_t sx, const int8_t sy, MovesVector& outMoves) const;
	void getRookMoves(const Color color, const int8_t sx, const int8_t sy, MovesVector& outMoves) const;
	void getKnightMoves(const Color color, const int8_t sx, const int8_t sy, MovesVector& outMoves) const;
	void getQueenMoves(const Color color, const int8_t sx, const int8_t sy, MovesVector& outMoves) const;
	void getPawnMoves(const Color color, const int8_t sx, const int8_t sy, MovesVector& outMoves) const;
	void getKingMoves(const Color color, const int8_t sx, const int8_t sy, MovesVector& outMoves) const;
	void updateBoardDataFromMove(const BoardMove move, BoardTile fromTile);
};

struct BoardHasher {
	inline constexpr size_t operator()(const ChessBoard& board) const {
		return board.getHash();
	}
};
	