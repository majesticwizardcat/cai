#include "game/board.h"

#include <iostream>
#include <functional>

void Move::printMove() const {
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

void Board::setupBoard() {
	for (int i = 0; i < m_width; ++i) {
		m_tiles[index(i, 1)].placePiece(PieceType::PAWN, Color::WHITE);
		m_tiles[index(i, 6)].placePiece(PieceType::PAWN, Color::BLACK);
	}
	for (int i = 0; i < m_width; ++i) {
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

void Board::printBoard() const {
	std::cout << "  ";
	for (int i = 0; i < m_width * 4 + 1; ++i) {
		std::cout << '*';
	}

	std::cout << '\n';
	for (int y = m_height - 1; y >= 0; --y) {
		std::cout << (y + 1) << " * ";
		for (int x = 0; x < m_width; ++x) {
			std::cout << m_tiles[index(x, y)].getPiece().getPieceChar();
			if (x < m_width - 1) {
				std::cout << " | ";
			}
		}
		std::cout << " *\n";
	}

	std::cout << "  ";
	for (int i = 0; i < m_width * 4 + 1; ++i) {
		std::cout << '*';
	}
	std::cout << '\n' << "   ";

	for (int i = 0; i < m_width; ++i) {
		std::cout << ' ' << (char) ('a' + i) << "  ";
	}
	std::cout << ' ' << std::endl;
}

void Board::playMove(const Move& move) {
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
		m_tiles[index(move.rookCastleTile.getX(),
			move.rookCastleTile.getY())].placePiece(PieceType::ROOK, pieceColor);
		return;
	}
	if (move.enPassantPawn.getPiece().getType() == PieceType::PAWN) {
		m_tiles[index(move.enPassantPawn.getX(), move.enPassantPawn.getY())].removePiece();
	}
}

std::vector<Move> Board::getKnightMoves(Color color, int sx, int sy) const {
	std::vector<Move> moves;
	const Tile& from = m_tiles[index(sx, sy)];
	for (int i = -1; i <= 1; ++i) {
		if (i == 0) {
			continue;
		}
		for (int j = -1; j <= 1; ++j) {
			if (j == 0) {
				continue;
			}
			if (sx + 2 * i >= 0 && sx + 2 * i < m_width
				&& sy + j >= 0 && sy + j < m_height) {
				const Tile& to = m_tiles[index(sx + 2 * i, sy + j)];
				if (to.getPiece().getType() == PieceType::NONE
					|| to.getPiece().getColor() != color) {
					Move m;
					m.from = from;
					m.to = to;
					moves.emplace_back(std::move(m));
				}
			}
			if (sy + 2 * i >= 0 && sy + 2 * i < m_width
				&& sx + j >= 0 && sx + j < m_height) {
				const Tile& to = m_tiles[index(sx + j, sy + i * 2)];
				if (to.getPiece().getType() == PieceType::NONE
					|| to.getPiece().getColor() != color) {
					Move m;
					m.from = from;
					m.to = to;
					moves.emplace_back(std::move(m));
				}
			}
		}
	}
	return std::move(moves);
}

std::vector<Move> Board::getBishopMoves(Color color, int sx, int sy) const {
	std::vector<Move> moves;
	for (int i = -1; i <= 1; ++i) {
		if (i == 0) {
			continue;
		}
		for (int j = -1; j <= 1; ++j) {
			if (j == 0) {
				continue;
			}
			std::vector<Move> bishopMoves = getDirectionMoves(color, sx, sy, i, j);
			std::move(bishopMoves.begin(), bishopMoves.end(), std::back_inserter(moves));
		}
	}
	return std::move(moves);
}

std::vector<Move> Board::getRookMoves(Color color, int sx, int sy) const {
	std::vector<Move> moves;
	for (int i = -1; i <= 1; ++i) {
		if (i == 0) {
			continue;
		}
		std::vector<Move> rookMoves = getDirectionMoves(color, sx, sy, i, 0);
		std::move(rookMoves.begin(), rookMoves.end(), std::back_inserter(moves));
		rookMoves = getDirectionMoves(color, sx, sy, 0, i);
		std::move(rookMoves.begin(), rookMoves.end(), std::back_inserter(moves));
	}
	return std::move(moves);
}

std::vector<Move> Board::getQueenMoves(Color color, int sx, int sy) const {
	std::vector<Move> moves;
	std::vector<Move> bishopMoves = getBishopMoves(color, sx, sy);
	std::vector<Move> rookMoves = getRookMoves(color, sx, sy);
	std::move(bishopMoves.begin(), bishopMoves.end(), std::back_inserter(moves));
	std::move(rookMoves.begin(), rookMoves.end(), std::back_inserter(moves));
	return std::move(moves);
}

std::vector<Move> Board::getKingMoves(Color color, int sx, int sy) const {
	std::vector<Move> moves;
	const Tile& from = m_tiles[index(sx, sy)];
	for (int i = -1; i <= 1; ++i) {
		for (int j = -1; j <= 1; ++j) {
			if ((i == 0 && j == 0)
				|| sx + i < 0 || sx + i >= m_width
				|| sy + j < 0 || sy + j >= m_height) {
				continue;
			}
			const Tile& to = m_tiles[index(sx + i, sy + j)];
			if (to.getPiece().getType() == PieceType::NONE
				|| to.getPiece().getColor() != color) {
				Move m;
				m.from = from;
				m.to = to;
				moves.emplace_back(std::move(m));
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
			Move m;
			m.from = from;
			m.to = m_tiles[index(from.getX() - 2, from.getY())];
			m.rookTile = m_tiles[index(0, from.getY())];
			m.rookCastleTile = m_tiles[index(from.getX() - 1, from.getY())];
			moves.emplace_back(std::move(m));
		}
		if (m_tiles[index(7, from.getY())].getPiece().getType() == PieceType::ROOK
			&& m_tiles[index(7, from.getY())].getPiece().getColor() == color
			&& m_tiles[index(7, from.getY())].getPiece().getMove() == 0
			&& empty(from.getX() + 1, 6)) {
			Move m;
			m.from = from;
			m.to = m_tiles[index(from.getX() + 2, from.getY())];
			m.rookTile = m_tiles[index(7, from.getY())];
			m.rookCastleTile = m_tiles[index(from.getX() + 1, from.getY())];
			moves.emplace_back(std::move(m));
		}
	}
	return std::move(moves);
}

std::vector<Move> Board::getMovesForPiece(Color color, int x, int y) const {
	switch(m_tiles[index(x, y)].getPiece().getType()) {
	case PieceType::PAWN:
		return std::move(getPawnMoves(color, x, y));
	case PieceType::KNIGHT:
		return std::move(getKnightMoves(color, x, y));
	case PieceType::BISHOP:
		return std::move(getBishopMoves(color, x, y));
	case PieceType::ROOK:
		return std::move(getRookMoves(color, x, y));
	case PieceType::QUEEN:
		return std::move(getQueenMoves(color, x, y));
	case PieceType::KING:
		return std::move(getKingMoves(color, x, y));
	default:
		return std::vector<Move>();
	}
	return std::vector<Move>();
}

std::vector<Move> Board::getMoves(Color color) const {
	std::vector<Move> moves;
	moves.reserve(128);
	for (int x = 0; x < m_width; ++x) {
		for (int y = 0; y < m_height; ++y) {
			if (m_tiles[index(x, y)].getPiece().getType() != PieceType::NONE
				&& m_tiles[index(x, y)].getPiece().getColor() == color) {
				std::vector<Move> pieceMoves = getMovesForPiece(color, x, y);
				std::move(pieceMoves.begin(), pieceMoves.end(), std::back_inserter(moves));
			}
		}
	}
	auto it = moves.begin();
	while(it != moves.end()) {
		if (!isMoveValid(*it)) {
			it = moves.erase(it);
		}
		else {
			it++;
		}
	}
	return std::move(moves);
}

bool Board::isMoveValid(const Move& m) const {
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

std::vector<Move> Board::getDirectionMoves(Color color, int sx, int sy, int dx, int dy) const {
	std::vector<Move> moves;
	int x = sx;
	int y = sy;
	while(true) {
		x += dx;
		y += dy;
		if (x < 0 || x >= m_width || y < 0 || y >= m_height) {
			break;
		}
		const Tile& to = m_tiles[index(x, y)];
		if (to.getPiece().getType() != PieceType::NONE) {
			if (to.getPiece().getColor() != color) {
				Move m;
				m.from = m_tiles[index(sx, sy)];
				m.to = to;
				moves.emplace_back(std::move(m));
			}
			break;
		}
		Move m;
		m.from = m_tiles[index(sx, sy)];
		m.to = to;
		moves.emplace_back(std::move(m));
	}
	return std::move(moves);
}

std::vector<Move> Board::addPromotionMoves(const Tile& from, const Tile& to) const {
	std::vector<Move> moves;
	moves.reserve(4);
	Move m;
	m.from = from;
	m.to = to;
	m.promotionType = PieceType::KNIGHT;
	moves.emplace_back(m);
	m.promotionType = PieceType::BISHOP;
	moves.emplace_back(m);
	m.promotionType = PieceType::ROOK;
	moves.emplace_back(m);
	m.promotionType = PieceType::QUEEN;
	moves.emplace_back(std::move(m));
	return std::move(moves);
}

std::vector<Move> Board::getPawnMoves(Color color, int sx, int sy) const {
	std::vector<Move> moves;
	const Tile& from = m_tiles[index(sx, sy)];
	int dir = color == Color::WHITE ? 1 : -1;
	int enPas = color == Color::WHITE ? 4 : 3;
	int prom = color == Color::WHITE ? 7 : 0;
	int pawnStart = color == Color::WHITE ? 1 : 6;
	for (int i = -1; i <= 1; ++i) {
		if (sx + i < 0 || sx + i >= m_width) {
			continue;
		}
		const Tile& to = m_tiles[index(sx + i, sy + dir)];
		PieceType type = to.getPiece().getType();
		Color clr = to.getPiece().getColor();
		if ((i == 0 && type == PieceType::NONE)
			|| (i != 0 && type != PieceType::NONE && clr != color)) {
			if (sy + dir == prom) {
				std::vector<Move> proms = addPromotionMoves(from, to);
				std::move(proms.begin(), proms.end(), std::back_inserter(moves));
			}
			else {
				Move m;
				m.from = from;
				m.to = to;
				moves.emplace_back(std::move(m));
			}
		}
	}
	if (sy == pawnStart
		&& sy + 2 * dir < m_height
		&& sy + 2 * dir >= 0
		&& m_tiles[index(sx, sy + 2 * dir)].getPiece().getType() == PieceType::NONE
		&& m_tiles[index(sx, sy + dir)].getPiece().getType() == PieceType::NONE) {
		Move m;
		m.from = from;
		m.to = m_tiles[index(sx, sy + 2 * dir)];
		moves.emplace_back(std::move(m));
	}
	if (sy == enPas) {
		for (int i = -1; i <= 1; ++i) {
			if (i == 0) {
				continue;
			}
			if (sx + i >= 0
				&& sx + i < m_width
				&& m_tiles[index(sx + i, sy)].getPiece().getType() == PieceType::PAWN
				&& m_tiles[index(sx + i, sy)].getPiece().getColor() != color
				&& m_tiles[index(sx + i, sy)].getPiece().getMove() == m_movesPlayed
				&& m_tiles[index(sx + i, sy)].getPiece().getLastMove() == 0
				&& m_tiles[index(sx + i, sy + dir)].getPiece().getType() == PieceType::NONE) {
				Move m;
				m.from = from;
				m.to = m_tiles[index(sx + i, sy + dir)];
				m.enPassantPawn = m_tiles[index(sx + i, sy)];
				moves.emplace_back(std::move(m));
			}

		}
	}
	return std::move(moves);
}

const Tile& Board::findKing(Color c) const {
	for (const Tile& tile : m_tiles) {
		if (tile.getPiece().getType() == PieceType::KING
			&& tile.getPiece().getColor() == c) {
			return tile;
		}
	}
	return m_tiles[0];
}

bool Board::isKingInCheck(Color color) const {
	const Tile& kingTile = findKing(color);
	std::vector<Move> moves = getBishopMoves(color, kingTile.getX(), kingTile.getY());
	for (const Move& m : moves) {
		if (m.to.getPiece().getType() == PieceType::QUEEN
			|| m.to.getPiece().getType() == PieceType::BISHOP) {
			return true;
		}
	}
	moves = getKnightMoves(color, kingTile.getX(), kingTile.getY());
	for (const Move& m : moves) {
		if (m.to.getPiece().getType() == PieceType::KNIGHT) {
			return true;
		}
	}
	moves = getRookMoves(color, kingTile.getX(), kingTile.getY());
	for (const Move& m : moves) {
		if (m.to.getPiece().getType() == PieceType::QUEEN
			|| m.to.getPiece().getType() == PieceType::ROOK) {
			return true;
		}
	}
	moves = getKingMoves(color, kingTile.getX(), kingTile.getY());
	for (const Move& m : moves) {
		if (m.to.getPiece().getType() == PieceType::KING) {
			return true;
		}
	}
	int dir = color == Color::WHITE ? 1 : -1;
	if (kingTile.getY() + dir >= 0 && kingTile.getY() + dir < m_height) {
		for (int i = -1; i <= 1; ++i) {
			if (i == 0 || kingTile.getX() + i < 0
				|| kingTile.getX() + i >= m_width) {
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

std::pair<bool, bool> Board::canCastle(Color color) const {
	std::pair<bool, bool> castles = std::make_pair(false, false);
	const Tile& kingTile = findKing(color);
	std::vector<Move> moves = getKingMoves(color, kingTile.getX(), kingTile.getY());
	for (const auto& m : moves) {
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

Color Board::setPosition(const std::string& fen) {
	m_tiles.clear();
	m_tiles.reserve(64);
	m_width = 8;
	m_height = 8;
	m_movesPlayed = 1;
	for (int x = 0; x < m_width; ++x) {
		for (int y = 0; y < m_height; ++y) {
			m_tiles.emplace_back(Tile(x, y));
		}
	}

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
		return std::move(piece);
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
		else {
			m_tiles[index(0, 7)].pieceMoved(0, 0);
			m_tiles[index(4, 7)].pieceMoved(0, 0);
		}
	}
	fenIndex++;
	if (fen[fenIndex] != '-') {
		int x = fen[fenIndex++] - 'a';
		int y = fen[fenIndex++] - '0';
		m_tiles[index(x, y)].pieceMoved(1, 0);
	}
	return nextPlayer;
}

