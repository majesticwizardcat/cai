#pragma once

#include <array>

enum class Color;
enum class PieceType;
class Piece;

enum class Color { WHITE, BLACK };
enum class PieceType { NONE, PAWN, KNIGHT, BISHOP, ROOK, QUEEN, KING };
static const std::array<PieceType, 7> PIECE_TYPES_ARRAY = {
	PieceType::NONE,
	PieceType::PAWN,
	PieceType::KNIGHT,
	PieceType::BISHOP,
	PieceType::ROOK,
	PieceType::QUEEN,
	PieceType::KING,
};

class Piece {
public:
	Piece() : m_type(PieceType::NONE), m_moves(0), m_lastMove(0) { }
	Piece(PieceType type, Color color) : m_type(type), m_color(color), m_moves(0),
		m_lastMove(0) { }
	Piece(const Piece& other) : m_type(other.m_type), m_color(other.m_color),
		m_moves(other.m_moves), m_lastMove(0) { }

	char getPieceChar() const {
		switch(m_type) {
		case PieceType::PAWN:
			return m_color == Color::WHITE ? 'P' : 'p';
		case PieceType::KNIGHT:
			return m_color == Color::WHITE ? 'N' : 'n';
		case PieceType::BISHOP:
			return m_color == Color::WHITE ? 'B' : 'b';
		case PieceType::ROOK:
			return m_color == Color::WHITE ? 'R' : 'r';
		case PieceType::QUEEN:
			return m_color == Color::WHITE ? 'Q' : 'q';
		case PieceType::KING:
			return m_color == Color::WHITE ? 'K' : 'k';
		default:
			return ' ';

		};
		return ' ';
	}

	char getPieceFloat() const {
		float color = m_color == Color::WHITE ? 1.0f : -1.0f;
		float start = 0.0f;
		if (m_moves == 0) {
			start = 0.05f;
		}
		switch(m_type) {
		case PieceType::PAWN:
			return (0.1f + start) * color;
		case PieceType::KNIGHT:
			return 0.3f * color;
		case PieceType::BISHOP:
			return 0.4f * color;
		case PieceType::ROOK:
			return (0.5f + start) * color;
		case PieceType::QUEEN:
			return (0.8f) * color;
		case PieceType::KING:
			return (1.0f + start) * color;
		default:
			return 0.0f;

		};
		return 0.0f;
	}
	
	inline PieceType getType() const { return m_type; }
	inline Color getColor() const {	return m_color; }
	inline void setType(PieceType type) { m_type = type; }
	inline void setColor(Color color) { m_color = color; }
	inline void move(int move, int last) { m_lastMove = last; m_moves = move; }
	inline int getMove() const { return m_moves; }
	inline int getLastMove() const { return m_lastMove; }

private:
	PieceType m_type;
	Color m_color; 
	int m_moves;
	int m_lastMove;
};
