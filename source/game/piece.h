#pragma once

enum class Color;
enum class PieceType;
class Piece;

enum class Color { WHITE, BLACK };
enum class PieceType { NONE, PAWN, KNIGHT, BISHOP, ROOK, QUEEN, KING };

class Piece {
private:
	PieceType m_type;
	Color m_color; 
	int m_moves;
	int m_lastMove;

public:
	Piece() : m_type(PieceType::NONE), m_moves(0), m_lastMove(0) { }
	Piece(PieceType type, Color color) : m_type(type), m_color(color), m_moves(0),
		m_lastMove(0) { }
	Piece(const Piece& other) : m_type(other.m_type), m_color(other.m_color),
		m_moves(other.m_moves), m_lastMove(0) { }

	char getPieceChar() const {
		switch(m_type) {
		case PieceType::PAWN:
			return m_color == Color::WHITE ? 'P' : 'n';
		case PieceType::KNIGHT:
			return m_color == Color::WHITE ? 'H' : 'h';
		case PieceType::BISHOP:
			return m_color == Color::WHITE ? 'B' : 'b';
		case PieceType::ROOK:
			return m_color == Color::WHITE ? 'R' : 'r';
		case PieceType::QUEEN:
			return m_color == Color::WHITE ? 'Q' : 'q';
		case PieceType::KING:
			return m_color == Color::WHITE ? 'K' : 'g';
		default:
			return ' ';

		};
		return ' ';
	}
	
	inline PieceType getType() const { return m_type; }
	inline Color getColor() const {	return m_color; }
	inline void setType(PieceType type) { m_type = type; }
	inline void setColor(Color color) { m_color = color; }
	inline void move(int move, int last) { m_lastMove = last; m_moves = move; }
	inline int getMove() const { return m_moves; }
	inline int getLastMove() const { return m_lastMove; }
};

