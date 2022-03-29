#pragma once

class Board;
struct Move;

#include <utility>
#include <vector>
#include <memory>

#include "game/piece.h"
#include "game/tile.h"

struct Move {
	Tile from;
	Tile to;
	Tile rookTile;
	Tile rookCastleTile;
	Tile enPassantPawn;
	PieceType promotionType;

	Move() : promotionType(PieceType::NONE) { }
	Move(const Tile& from, const Tile& to) : from(from), to(to) { }
	Move(const Move& other) : from(other.from), to(other.to), rookTile(other.rookTile),
		rookCastleTile(other.rookCastleTile), enPassantPawn(other.enPassantPawn),
		promotionType(other.promotionType) { }

	Move& operator=(const Move& other) {
		from = std::move(other.from);
		to = std::move(other.to);
		rookTile = std::move(other.rookTile);
		rookCastleTile = std::move(other.rookCastleTile);
		enPassantPawn = std::move(other.enPassantPawn);
		promotionType = std::move(other.promotionType);
		return *this;
	}

	void printMove() const;
};

class Board {
public:
	Board() : Board(8, 8) { }
	Board(int width, int height) : m_width(width), m_height(height), m_movesPlayed(0) {
		m_tiles.reserve(m_width * m_height);
		for (int x = 0; x < m_width; ++x) {
			for (int y = 0; y < m_height; ++y) {
				m_tiles.emplace_back(Tile(x, y));
			}
		}
	}

	Board(const Board& other) : m_width(other.m_width), m_height(other.m_height),
		m_tiles(other.m_tiles), m_movesPlayed(other.m_movesPlayed) { }
	std::vector<Move> getMoves(Color color) const;
	void setupBoard();
	void printBoard() const;
	void playMove(const Move& move);

	inline void placePiece(int x, int y, PieceType type, Color color) {
		m_tiles[index(x, y)].placePiece(type, color);
	}

	bool isKingInCheck(Color color) const;
	bool isMoveValid(const Move& m) const;
	std::pair<bool, bool> canCastle(Color color) const;
	bool isDraw() const;

	inline int movesPlayed() const { return m_movesPlayed; }

	inline std::string asString() const {
		std::string ret;
		ret.reserve(m_width * m_height);
		for (const auto& t : m_tiles) {
			ret.push_back(t.getPiece().getPieceChar());
		}
		return std::move(ret);
	}

	inline std::vector<float> asFloats() const {
		std::vector<float> result;
		result.reserve(m_width * m_height);
		for (const auto& t : m_tiles) {
			result.push_back(t.getPiece().getPieceFloat());
		}
		return std::move(result);
	}

	Color setPosition(const std::string& fen);

private:
	int m_width;
	int m_height;
	int m_movesPlayed;
	std::vector<Tile> m_tiles;

	inline int index(int x, int y) const { return x * m_height + y; }
	std::vector<Move> getDirectionMoves(Color color, int sx, int sy, int dx, int dy) const;
	std::vector<Move> getPawnMoves(Color color, int sx, int sy) const;
	std::vector<Move> getKnightMoves(Color color, int sx, int sy) const;
	std::vector<Move> getBishopMoves(Color color, int sx, int sy) const;
	std::vector<Move> getRookMoves(Color color, int sx, int sy) const;
	std::vector<Move> getQueenMoves(Color color, int sx, int sy) const;
	std::vector<Move> getKingMoves(Color color, int sx, int sy) const;
	std::vector<Move> getMovesForPiece(Color color, int x, int y) const;
	std::vector<Move> addPromotionMoves(const Tile& from, const Tile& to) const;
	const Tile& findKing(Color color) const;
};

