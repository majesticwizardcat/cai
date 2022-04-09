#pragma once

template<unsigned int Width, unsigned int Height> class Board;
struct Move;

#include <utility>
#include <vector>
#include <memory>
#include <iostream>
#include <functional>
#include <array>

#include <nnpp.hpp>

#include "game/piece.h"
#include "game/tile.h"

typedef Board<8, 8> ChessBoard;
typedef StackVector<Move, 256> MovesStackVector;
typedef std::array<float, 8 * 8> FloatChessBoard;

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

	Move& operator=(Move&& other) {
		from = std::move(other.from);
		to = std::move(other.to);
		rookTile = std::move(other.rookTile);
		rookCastleTile = std::move(other.rookCastleTile);
		enPassantPawn = std::move(other.enPassantPawn);
		promotionType = std::move(other.promotionType);
		return *this;
	}

	void printMove() const {
		auto printPromotion = [&](PieceType type) {
			switch(type) {
			case PieceType::KNIGHT:
				return 'K';
			case PieceType::BISHOP:
				return 'B';
			case PieceType::ROOK:
				return 'R';
			case PieceType::QUEEN:
				return 'Q';
			default:
				return '\0';
			}
			return '\0';
		};

		if (rookTile.getPiece().getType() == PieceType::ROOK) {
			if (rookTile.getX() == 0) {
				std::cout << "O--O";
				return;
			}
			std::cout << "O-O";
			return;
		}
		std::cout << from.getPiece().getPieceChar()
			<< from.tileString() << " -> "
			<< to.tileString()
			<< printPromotion(promotionType);
	}
};

template<unsigned int Width, unsigned int Height> class Board {
public:
	Board()
		: m_movesPlayed(0) {
		createNewBoard();
	}

	Board(const Board& other) 
		: m_movesPlayed(other.m_movesPlayed)
		, m_tiles(other.m_tiles) { }

	void getMoves(Color color, MovesStackVector* outMoves) const {
		for (int x = 0; x < Width; ++x) {
			for (int y = 0; y < Height; ++y) {
				if (m_tiles[index(x, y)].getPiece().getType() != PieceType::NONE
					&& m_tiles[index(x, y)].getPiece().getColor() == color) {
					getMovesForPiece(color, x, y, outMoves);
				}
			}
		}

		unsigned int i = 0;
		while(i < outMoves->size()) {
			if (!isMoveValid((*outMoves)[i])) {
				outMoves->erase(i);
			}
			else {
				i++;
			}
		}
	}

	void setupBoard() {
		for (int i = 0; i < Width; ++i) {
			m_tiles[index(i, 1)].placePiece(PieceType::PAWN, Color::WHITE);
			m_tiles[index(i, 6)].placePiece(PieceType::PAWN, Color::BLACK);
		}

		for (int i = 0; i < Width; ++i) {
			if (i == 0 || i == 7) {
				m_tiles[index(i, 0)].placePiece(PieceType::ROOK, Color::WHITE);
				m_tiles[index(i, 7)].placePiece(PieceType::ROOK, Color::BLACK);
			}
			else if (i == 1 || i == 6) {
				m_tiles[index(i, 0)].placePiece(PieceType::KNIGHT, Color::WHITE);
				m_tiles[index(i, 7)].placePiece(PieceType::KNIGHT, Color::BLACK);
			}
			else if (i == 2 || i == 5) {
				m_tiles[index(i, 0)].placePiece(PieceType::BISHOP, Color::WHITE);
				m_tiles[index(i, 7)].placePiece(PieceType::BISHOP, Color::BLACK);
			}
			else if (i == 3) {
				m_tiles[index(i, 0)].placePiece(PieceType::QUEEN, Color::WHITE);
				m_tiles[index(i, 7)].placePiece(PieceType::QUEEN, Color::BLACK);
			}
			else {
				m_tiles[index(i, 0)].placePiece(PieceType::KING, Color::WHITE);
				m_tiles[index(i, 7)].placePiece(PieceType::KING, Color::BLACK);
			}
		}
	}

	void printBoard() const {
		std::cout << "  ";
		for (int i = 0; i < Width * 4 + 1; ++i) {
			std::cout << '*';
		}

		std::cout << '\n';
		for (int y = Height - 1; y >= 0; --y) {
			std::cout << (y + 1) << " * ";
			for (int x = 0; x < Width; ++x) {
				std::cout << m_tiles[index(x, y)].getPiece().getPieceChar();
				if (x < Width - 1) {
					std::cout << " | ";
				}
			}
			std::cout << " *\n";
		}

		std::cout << "  ";
		for (int i = 0; i < Width * 4 + 1; ++i) {
			std::cout << '*';
		}
		std::cout << '\n' << "   ";

		for (int i = 0; i < Width; ++i) {
			std::cout << ' ' << (char) ('a' + i) << "  ";
		}
		std::cout << ' ' << std::endl;
	}

	void playMove(const Move& move) {
		Color pieceColor = move.from.getPiece().getColor();
		Tile& fromTile = m_tiles[index(move.from.getX(), move.from.getY())];
		Tile& toTile = m_tiles[index(move.to.getX(), move.to.getY())];
		m_movesPlayed++;
		toTile.pieceMoved(m_movesPlayed, fromTile.getPiece().getMove());
		fromTile.removePiece();
		if (move.promotionType != PieceType::NONE) {
			toTile.placePiece(move.promotionType, pieceColor);
			return;
		}

		PieceType pieceType = move.from.getPiece().getType();
		toTile.placePiece(pieceType, pieceColor);
		if (move.rookTile.getPiece().getType() == PieceType::ROOK) {
			m_tiles[index(move.rookTile.getX(), move.rookTile.getY())].removePiece();
			m_tiles[index(move.rookCastleTile.getX(), move.rookCastleTile.getY())].placePiece(PieceType::ROOK, pieceColor);
			return;
		}

		if (move.enPassantPawn.getPiece().getType() == PieceType::PAWN) {
			m_tiles[index(move.enPassantPawn.getX(), move.enPassantPawn.getY())].removePiece();
		}
	}

	inline void placePiece(int x, int y, PieceType type, Color color) {
		m_tiles[index(x, y)].placePiece(type, color);
	}

	bool isDraw() const {
		for (const auto& tile : m_tiles) {
			if (tile.getPiece().getType() != PieceType::NONE && tile.getPiece().getType() != PieceType::KING) {
				return false;
			}
		}
		return true;
	}

	std::pair<bool, bool> canCastle(Color color) const {
		std::pair<bool, bool> castles = std::make_pair(false, false);
		const Tile& kingTile = findKing(color);
		MovesStackVector moves;
		getKingMoves(color, kingTile.getX(), kingTile.getY(), &moves);
		for (const Move& m : moves) {
			if (m.rookTile.getPiece().getType() != PieceType::NONE) {
				if (m.rookTile.getX() == 0) {
					castles.second = true;
				}
				else if (m.rookTile.getX() == 7) {
					castles.first = true;
				}
			}
		}
		return castles;
	}

	bool isKingInCheck(Color color) const {
		MovesStackVector moves;
		const Tile& kingTile = findKing(color);
		getBishopMoves(color, kingTile.getX(), kingTile.getY(), &moves);
		for (const Move& m : moves) {
			if (m.to.getPiece().getType() == PieceType::QUEEN
				|| m.to.getPiece().getType() == PieceType::BISHOP) {
				return true;
			}
		}

		moves.clear();
		getKnightMoves(color, kingTile.getX(), kingTile.getY(), &moves);
		for (const Move& m : moves) {
			if (m.to.getPiece().getType() == PieceType::KNIGHT) {
				return true;
			}
		}

		moves.clear();
		getRookMoves(color, kingTile.getX(), kingTile.getY(), &moves);
		for (const Move& m : moves) {
			if (m.to.getPiece().getType() == PieceType::QUEEN
				|| m.to.getPiece().getType() == PieceType::ROOK) {
				return true;
			}
		}

		moves.clear();
		getKingMoves(color, kingTile.getX(), kingTile.getY(), &moves);
		for (const Move& m : moves) {
			if (m.to.getPiece().getType() == PieceType::KING) {
				return true;
			}
		}

		int dir = color == Color::WHITE ? 1 : -1;
		if (kingTile.getY() + dir >= 0 && kingTile.getY() + dir < Height) {
			for (int i = -1; i <= 1; ++i) {
				if (i == 0 || kingTile.getX() + i < 0
					|| kingTile.getX() + i >= Width) {
					continue;
				}
				const Tile& to = m_tiles[index(kingTile.getX() + i, kingTile.getY() + dir)];
				if (to.getPiece().getType() == PieceType::PAWN
					&& to.getPiece().getColor() != color) {
					return true;
				}
			}
		}
		return false;
	}

	inline int movesPlayed() const { return m_movesPlayed; }

	inline std::string asString() const {
		std::string ret;
		ret.reserve(Width * Height);
		for (const auto& t : m_tiles) {
			ret.push_back(t.getPiece().getPieceChar());
		}
		return std::move(ret);
	}

	inline NNPPStackVector<float> asFloats() const {
		NNPPStackVector<float> result;
		for (const auto& t : m_tiles) {
			result.push(t.getPiece().getPieceFloat());
		}
		return result;
	}

	Color setPosition(const std::string& fen) {
		createNewBoard();
		m_movesPlayed = 1;

		auto charToPiece = [](char c) {
			std::pair<PieceType, Color> piece;
			piece.second = c >= 'a' && c <= 'z' ? Color::BLACK : Color::WHITE;
			c = c >= 'A' && c <= 'Z' ? 'a' + c - 'A' : c;
			switch(c) {
			case 'r':
				piece.first = PieceType::ROOK;
				break;
			case 'b':
				piece.first = PieceType::BISHOP;
				break;
			case 'n':
				piece.first = PieceType::KNIGHT;
				break;
			case 'q':
				piece.first = PieceType::QUEEN;
				break;
			case 'k':
				piece.first = PieceType::KING;
				break;
			default:
				piece.first = PieceType::PAWN;
				break;
			}
			return piece;
		};

		int fenIndex = 0;
		for (int y = 0; y < 8; ++y) {
			for (int x = 0; x < 8; ++x) {
				char next = fen[fenIndex++];
				if (next >= '0' && next <= '9') {
					x += next - '0' - 1;
				}
				else {
					auto piece = charToPiece(next);
					assert(x >= 0);
					m_tiles[index(x, 7 - y)].placePiece(piece.first, piece.second);
					if (piece.first == PieceType::ROOK || piece.first == PieceType::KING) {
						m_tiles[index(x, 7 - y)].pieceMoved(1, 0);
					}
					else if (piece.first == PieceType::PAWN) {
						if ((piece.second == Color::WHITE && 7 - y == 1)
							|| (piece.second == Color::BLACK && 7 - y == 6)) {
							m_tiles[index(x, 7 - y)].pieceMoved(0, 0);
						}
						else {
							m_tiles[index(x, 7 - y)].pieceMoved(1, 1);
						}
					}
					else {
						m_tiles[index(x, 7 - y)].pieceMoved(0, 0);
					}
				}
			}
			fenIndex++;
		}

		Color nextPlayer = fen[fenIndex++] == 'w' ? Color::WHITE : Color::BLACK;
		fenIndex++;
		for (int i = 0; i < 4; ++i) {
			char next = fen[fenIndex++];
			if (next == '-' || next == ' ') {
				break;
			}
			if (next == 'K') {
				m_tiles[index(7, 0)].pieceMoved(0, 0);
				m_tiles[index(4, 0)].pieceMoved(0, 0);
			}
			else if (next == 'Q') {
				m_tiles[index(0, 0)].pieceMoved(0, 0);
				m_tiles[index(4, 0)].pieceMoved(0, 0);
			}
			else if (next == 'k') {
				m_tiles[index(7, 7)].pieceMoved(0, 0);
				m_tiles[index(4, 7)].pieceMoved(0, 0);
			}
			else if (next == 'q') {
				m_tiles[index(0, 7)].pieceMoved(0, 0);
				m_tiles[index(4, 7)].pieceMoved(0, 0);
			}
			else {
				assert(false);
			}
		}
		fenIndex++;
		if (fen[fenIndex] != '-' && fen[fenIndex] != ' ') {
			int x = fen[fenIndex++] - 'a';
			int y = fen[fenIndex++] - '0';
			m_tiles[index(x, y)].pieceMoved(1, 0);
		}
		return nextPlayer;
	}

	bool isMoveValid(const Move& m) const {
		Board b(*this);
		b.playMove(m);
		if (b.isKingInCheck(m.from.getPiece().getColor())) {
			return false;
		}

		if (m.rookTile.getPiece().getType() == PieceType::ROOK) {
			if (isKingInCheck(m.from.getPiece().getColor())) {
				return false;
			}
			if (m.rookTile.getX() == 0) {
				Move mid;
				mid.from = m.from;
				mid.to = m_tiles[index(m.from.getX() - 1, m.from.getY())];
				Board midBoard(*this);
				midBoard.playMove(mid);
				return !midBoard.isKingInCheck(m.from.getPiece().getColor());
			}
			else {
				Move mid;
				mid.from = m.from;
				mid.to = m_tiles[index(m.from.getX() + 1, m.from.getY())];
				Board midBoard(*this);
				midBoard.playMove(mid);
				return !midBoard.isKingInCheck(m.from.getPiece().getColor());
			}
		}
		return true;
	}

private:
	int m_movesPlayed;
	std::array<Tile, Width * Height> m_tiles;

	inline unsigned int index(int x, int y) const throw() {
		assert(x * Height + y < Width * Height);
		return x * Height + y;
	}

	const Tile& findKing(Color c) const {
		for (const Tile& tile : m_tiles) {
			if (tile.getPiece().getType() == PieceType::KING
				&& tile.getPiece().getColor() == c) {
				return tile;
			}
		}
		assert(false);
		return m_tiles[0];
	}

	void getPawnMoves(Color color, int sx, int sy, MovesStackVector* outMoves) const {
		const Tile& from = m_tiles[index(sx, sy)];
		int dir = color == Color::WHITE ? 1 : -1;
		int enPas = color == Color::WHITE ? 4 : 3;
		int prom = color == Color::WHITE ? 7 : 0;
		int pawnStart = color == Color::WHITE ? 1 : 6;

		for (int i = -1; i <= 1; ++i) {
			if (sx + i < 0 || sx + i >= Width) {
				continue;
			}
			const Tile& to = m_tiles[index(sx + i, sy + dir)];
			PieceType type = to.getPiece().getType();
			Color clr = to.getPiece().getColor();
			if ((i == 0 && type == PieceType::NONE)
				|| (i != 0 && type != PieceType::NONE && clr != color)) {
				if (sy + dir == prom) {
					addPromotionMoves(from, to, outMoves);
				}
				else {
					Move& m = outMoves->addNew();
					m.from = from;
					m.to = to;
				}
			}
		}

		if (sy == pawnStart
			&& sy + 2 * dir < Height
			&& sy + 2 * dir >= 0
			&& m_tiles[index(sx, sy + 2 * dir)].getPiece().getType() == PieceType::NONE
			&& m_tiles[index(sx, sy + dir)].getPiece().getType() == PieceType::NONE) {
			Move& m = outMoves->addNew();
			m.from = from;
			m.to = m_tiles[index(sx, sy + 2 * dir)];
		}

		if (sy == enPas) {
			for (int i = -1; i <= 1; ++i) {
				if (i == 0) {
					continue;
				}
				if (sx + i >= 0
					&& sx + i < Width
					&& m_tiles[index(sx + i, sy)].getPiece().getType() == PieceType::PAWN
					&& m_tiles[index(sx + i, sy)].getPiece().getColor() != color
					&& m_tiles[index(sx + i, sy)].getPiece().getMove() == m_movesPlayed
					&& m_tiles[index(sx + i, sy)].getPiece().getLastMove() == 0
					&& m_tiles[index(sx + i, sy + dir)].getPiece().getType() == PieceType::NONE) {
					Move& m = outMoves->addNew();
					m.from = from;
					m.to = m_tiles[index(sx + i, sy + dir)];
					m.enPassantPawn = m_tiles[index(sx + i, sy)];
				}
			}
		}
	}

	void addPromotionMoves(const Tile& from, const Tile& to, MovesStackVector* outMoves) const {
		for (PieceType type : PIECE_TYPES_ARRAY) {
			if (type != PieceType::KNIGHT
				&& type != PieceType::BISHOP
				&& type != PieceType::ROOK
				&& type != PieceType::QUEEN) {
				continue;
			}
			Move& m = outMoves->addNew();
			m.from = from;
			m.to = to;
			m.promotionType = type;
		}
	}

	void getDirectionMoves(Color color, int sx, int sy, int dx, int dy, MovesStackVector* outMoves) const {
		int x = sx;
		int y = sy;
		while(true) {
			x += dx;
			y += dy;
			if (x < 0 || x >= Width || y < 0 || y >= Height) {
				break;
			}

			const Tile& to = m_tiles[index(x, y)];
			if (to.getPiece().getType() != PieceType::NONE) {
				if (to.getPiece().getColor() != color) {
					Move& m = outMoves->addNew();
					m.from = m_tiles[index(sx, sy)];
					m.to = to;
				}
				break;
			}

			Move& m = outMoves->addNew();
			m.from = m_tiles[index(sx, sy)];
			m.to = to;
		}
	}

	void getMovesForPiece(Color color, int x, int y, MovesStackVector* outMoves) const {
		switch(m_tiles[index(x, y)].getPiece().getType()) {
		case PieceType::PAWN:
			getPawnMoves(color, x, y, outMoves);
			break;
		case PieceType::KNIGHT:
			getKnightMoves(color, x, y, outMoves);
			break;
		case PieceType::BISHOP:
			getBishopMoves(color, x, y, outMoves);
			break;
		case PieceType::ROOK:
			getRookMoves(color, x, y, outMoves);
			break;
		case PieceType::QUEEN:
			getQueenMoves(color, x, y, outMoves);
			break;
		case PieceType::KING:
			getKingMoves(color, x, y, outMoves);
			break;
		default:
			assert(false);
		}
	}

	void getKnightMoves(Color color, int sx, int sy, MovesStackVector* outMoves) const {
		const Tile& from = m_tiles[index(sx, sy)];
		for (int i = -1; i <= 1; ++i) {
			if (i == 0) {
				continue;
			}

			for (int j = -1; j <= 1; ++j) {
				if (j == 0) {
					continue;
				}

				if (sx + 2 * i >= 0 && sx + 2 * i < Width
					&& sy + j >= 0 && sy + j < Height) {
					const Tile& to = m_tiles[index(sx + 2 * i, sy + j)];
					if (to.getPiece().getType() == PieceType::NONE
						|| to.getPiece().getColor() != color) {
						Move& m = outMoves->addNew();
						m.from = from;
						m.to = to;
					}
				}

				if (sy + 2 * i >= 0 && sy + 2 * i < Width
					&& sx + j >= 0 && sx + j < Height) {
					const Tile& to = m_tiles[index(sx + j, sy + i * 2)];
					if (to.getPiece().getType() == PieceType::NONE
						|| to.getPiece().getColor() != color) {
						Move& m = outMoves->addNew();
						m.from = from;
						m.to = to;
					}
				}
			}
		}
	}

	void getBishopMoves(Color color, int sx, int sy, MovesStackVector* outMoves) const {
		for (int i = -1; i <= 1; ++i) {
			if (i == 0) {
				continue;
			}

			for (int j = -1; j <= 1; ++j) {
				if (j == 0) {
					continue;
				}
				getDirectionMoves(color, sx, sy, i, j, outMoves);
			}
		}
	}

	void getRookMoves(Color color, int sx, int sy, MovesStackVector* outMoves) const {
		for (int i = -1; i <= 1; ++i) {
			if (i == 0) {
				continue;
			}
			getDirectionMoves(color, sx, sy, i, 0, outMoves);
			getDirectionMoves(color, sx, sy, 0, i, outMoves);
		}
	}

	void getQueenMoves(Color color, int sx, int sy, MovesStackVector* outMoves) const {
		getBishopMoves(color, sx, sy, outMoves);
		getRookMoves(color, sx, sy, outMoves);
	}

	void getKingMoves(Color color, int sx, int sy, MovesStackVector* outMoves) const {
		const Tile& from = m_tiles[index(sx, sy)];
		for (int i = -1; i <= 1; ++i) {
			for (int j = -1; j <= 1; ++j) {
				if ((i == 0 && j == 0)
					|| sx + i < 0 || sx + i >= Width
					|| sy + j < 0 || sy + j >= Height) {
					continue;
				}

				const Tile& to = m_tiles[index(sx + i, sy + j)];
				if (to.getPiece().getType() == PieceType::NONE
					|| to.getPiece().getColor() != color) {
					Move& m = outMoves->addNew();
					m.from = from;
					m.to = to;
				}
			}
		}

		auto empty = [&](int start, int end) {
			for (int i = start; i <= end; ++i) {
				if (m_tiles[index(i, from.getY())].getPiece().getType() != PieceType::NONE){
					return false;
				}
			}
			return true;
		};

		if (from.getPiece().getMove() == 0) {
			if (m_tiles[index(0, from.getY())].getPiece().getType() == PieceType::ROOK
				&& m_tiles[index(0, from.getY())].getPiece().getColor() == color
				&& m_tiles[index(0, from.getY())].getPiece().getMove() == 0
				&& empty(1, from.getX() - 1)) {
				Move& m = outMoves->addNew();
				m.from = from;
				m.to = m_tiles[index(from.getX() - 2, from.getY())];
				m.rookTile = m_tiles[index(0, from.getY())];
				m.rookCastleTile = m_tiles[index(from.getX() - 1, from.getY())];
			}

			if (m_tiles[index(7, from.getY())].getPiece().getType() == PieceType::ROOK
				&& m_tiles[index(7, from.getY())].getPiece().getColor() == color
				&& m_tiles[index(7, from.getY())].getPiece().getMove() == 0
				&& empty(from.getX() + 1, 6)) {
				Move& m = outMoves->addNew();
				m.from = from;
				m.to = m_tiles[index(from.getX() + 2, from.getY())];
				m.rookTile = m_tiles[index(7, from.getY())];
				m.rookCastleTile = m_tiles[index(from.getX() + 1, from.getY())];
			}
		}
	}

	inline void createNewBoard() {
		for (int x = 0; x < Width; ++x) {
			for (int y = 0; y < Height; ++y) {
				m_tiles[index(x, y)] = Tile(x, y);
			}
		}
	}
};
