#pragma once

class Tile;

#include "game/board.h"

#include <string>

class Tile {
private:
	int m_x;
	int m_y;
	Piece m_piece;

public:
	Tile() : m_x(0), m_y(0) { }
	Tile(int x, int y) : m_x(x), m_y(y) { }
	Tile(int x, int y, PieceType type, Color color) : m_x(x), m_y(y), m_piece(type, color) { }
	Tile(int x, int y, const Piece& piece) : m_x(x), m_y(y), m_piece(piece) { }
	Tile(const Tile& other) : m_x(other.m_x), m_y(other.m_y), m_piece(other.m_piece) { }

	inline Tile& operator=(Tile other) {
		std::swap(m_x, other.m_x);
		std::swap(m_y, other.m_y);
		std::swap(m_piece, other.m_piece);
		return *this;
	}

	inline void placePiece(PieceType piece, Color color) {
		m_piece.setType(piece);
		m_piece.setColor(color);
	}

	inline void removePiece() { m_piece.setType(PieceType::NONE); }
	inline const Piece& getPiece() const { return m_piece; }
	inline int getX() const { return m_x; }
	inline int getY() const { return m_y; }
	inline void pieceMoved(int move, int last) { m_piece.move(move, last); }
	inline std::string tileString() const {
		std::string str = { 'a', static_cast<char>(m_x), '1', static_cast<char>(m_y) };
		return str;
	}
};

