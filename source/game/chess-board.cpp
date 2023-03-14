#include "game/chess-board.h"

#include <iostream>

ChessBoard::ChessBoard()
		: m_positionData(0) {
	memset(m_tileData, 0, sizeof(m_tileData));
	m_positionInfo.enPassantSquare = TileCoords(INVALID, INVALID);
	m_positionInfo.canBlackShortCastle = true;
	m_positionInfo.canWhiteShortCastle = true;
	m_positionInfo.canBlackLongCastle = true;
	m_positionInfo.canWhiteLongCastle = true;
	m_positionInfo.nextPlayerColor = WHITE;

	for (uint8_t i = 0; i < BOARD_SIZE; ++i) {
		setTile(i, 1, BoardTile(WHITE, PAWN));
		setTile(i, 6, BoardTile(BLACK, PAWN));
	}

	for (uint8_t i = 0; i < BOARD_SIZE; ++i) {
		if (i == 0 || i == 7) {
			setTile(i, 0, BoardTile(WHITE, ROOK));
			setTile(i, 7, BoardTile(BLACK, ROOK));
		}
		else if (i == 1 || i == 6) {
			setTile(i, 0, BoardTile(WHITE, KNIGHT));
			setTile(i, 7, BoardTile(BLACK, KNIGHT));
		}
		else if (i == 2 || i == 5) {
			setTile(i, 0, BoardTile(WHITE, BISHOP));
			setTile(i, 7, BoardTile(BLACK, BISHOP));
		}
		else if (i == 3) {
			setTile(i, 0, BoardTile(WHITE, QUEEN));
			setTile(i, 7, BoardTile(BLACK, QUEEN));
		}
		else {
			setTile(i, 0, BoardTile(WHITE, KING));
			setTile(i, 7, BoardTile(BLACK, KING));
		}
	}
}

ChessBoard::ChessBoard(const std::string& fen)
		: m_positionData(0) {
	memset(m_tileData, 0, sizeof(m_tileData));
	m_positionInfo.enPassantSquare = TileCoords(INVALID, INVALID);
	m_positionInfo.canBlackShortCastle = false;
	m_positionInfo.canWhiteShortCastle = false;
	m_positionInfo.canBlackLongCastle = false;
	m_positionInfo.canWhiteLongCastle = false;

	auto charToTile = [](char c) {
		BoardTile tile;
		tile.color = c >= 'a' && c <= 'z' ? BLACK : WHITE;
		c = c >= 'A' && c <= 'Z' ? 'a' + c - 'A' : c;
		switch(c) {
		case 'r':
			tile.type = ROOK;
			break;
		case 'b':
			tile.type = BISHOP;
			break;
		case 'n':
			tile.type = KNIGHT;
			break;
		case 'q':
			tile.type = QUEEN;
			break;
		case 'k':
			tile.type = KING;
			break;
		default:
			tile.type = PAWN;
			break;
		}
		return tile;
	};

	uint32_t fenIndex = 0;
	for (uint32_t y = 0; y < BOARD_SIZE; ++y) {
		for (uint32_t x = 0; x < BOARD_SIZE; ++x) {
			char next = fen[fenIndex++];
			if (next >= '0' && next <= '9') {
				x += next - '0' - 1;
				assert(x <= BOARD_SIZE);
			}
			else {
				BoardTile tile = charToTile(next);
				setTile(x, 7 - y, tile);
			}
		}
		fenIndex++;
	}

	m_positionInfo.nextPlayerColor = fen[fenIndex++] == 'w' ? WHITE : BLACK;
	fenIndex++;
	for (uint8_t i = 0; i < 4; ++i) {
		char next = fen[fenIndex++];
		if (next == '-' || next == ' ') {
			break;
		}
		if (next == 'K') {
			m_positionInfo.canWhiteShortCastle = true;
		}
		else if (next == 'Q') {
			m_positionInfo.canWhiteLongCastle = true;
		}
		else if (next == 'k') {
			m_positionInfo.canBlackShortCastle = true;
		}
		else if (next == 'q') {
			m_positionInfo.canBlackLongCastle = true;
		}
		else {
			assert(false);
		}
	}

	fenIndex++;
	if (fen[fenIndex] != '-' && fen[fenIndex] != ' ') {
		int8_t x = fen[fenIndex++] - 'a';
		int8_t y = fen[fenIndex++] - '0';
		m_positionInfo.enPassantSquare = TileCoords(x, y);
		assert(m_positionInfo.enPassantSquare.areValid());
	}
}

void ChessBoard::printBoard() const {
	std::cout << "  ";
	for (uint32_t i = 0; i < BOARD_SIZE * 4 + 1; ++i) {
		std::cout << '*';
	}

	std::cout << '\n';
	for (uint8_t y = BOARD_SIZE - 1; y < BOARD_SIZE; --y) {
		std::cout << (y + 1) << " * ";
		for (uint8_t x = 0; x < BOARD_SIZE; ++x) {
			std::cout << static_cast<char>(getTile(x, y));
			if (x < BOARD_SIZE - 1) {
				std::cout << " | ";
			}
		}
		std::cout << " *\n";
	}

	std::cout << "  ";
	for (int i = 0; i < BOARD_SIZE * 4 + 1; ++i) {
		std::cout << '*';
	}
	std::cout << '\n' << "   ";

	for (int i = 0; i < BOARD_SIZE; ++i) {
		std::cout << ' ' << static_cast<char>('a' + i) << "  ";
	}
	std::cout << ' ' << std::endl;
}

void ChessBoard::printMoveOnBoard(const BoardMove& move) const {
	auto printPromotion = [](TileType type) {
		switch(type) {
		case KNIGHT:
			return 'K';
		case BISHOP:
			return 'B';
		case ROOK:
			return 'R';
		case QUEEN:
			return 'Q';
		default:
			return ' ';
		}
		return ' ';
	};
	const BoardTile from = getTile(move.from);
	const BoardTile to = getTile(move.to);

	if (move.isCastle(from.type)) {
		if (move.to.x == KING_LONG_CASTLE_X) {
			std::cout << "O--O" << '\n';
		}
		else {
			std::cout << "O-O" << '\n';
		}
		return;
	}

	std::cout << static_cast<char>(from) << static_cast<char>('a' + move.from.x) << static_cast<char>('1' + move.from.y)
		<< " -> " << static_cast<char>('a' + move.to.x) << static_cast<char>('1' + move.to.y) << printPromotion(move.promotionType) << '\n';
}

void ChessBoard::getMoves(Color color, MovesVector& outMoves) const {
	for (uint8_t x = 0; x < BOARD_SIZE; ++x) {
		for (uint8_t y = 0; y < BOARD_SIZE; ++y) {
			const BoardTile tile = getTile(x, y);
			if (tile.type != EMPTY && tile.color == color) {
				getMovesForPiece(tile, x, y, outMoves);
			}
		}
	}

	uint32_t index = 0;
	while (index < outMoves.size()) {
		if (!isMoveValid(outMoves[index])) {
			outMoves.erase(index);
		}
		else {
			index++;
		}
	}
}

void ChessBoard::playMove(const BoardMove& move) {
	assert(getTile(move.to).type != KING); // There should not be a move played that captures the king

	BoardTile from = getTile(move.from);
	if (move.promotionType != EMPTY) {
		from.type = move.promotionType;
	}

	m_positionInfo.enPassantSquare = TileCoords(INVALID, INVALID);

	if (move.enPassantPawn.areValid()) {
		assert(from.type == PAWN && move.enPassantPawn.areValid() && getTile(move.enPassantPawn).type == PAWN);
		removePiece(move.enPassantPawn);
	}
	else if (from.type == PAWN && std::abs(move.from.y - move.to.y) == 2) {
		m_positionInfo.enPassantSquare = move.to;
	}
	else if (from.type == KING) {
		if (from.color == WHITE) {
			m_positionInfo.canWhiteShortCastle = false;
			m_positionInfo.canWhiteLongCastle = false;
		}
		else {
			m_positionInfo.canBlackShortCastle = false;
			m_positionInfo.canBlackLongCastle = false;
		}

		if (move.isCastle(KING)) {
			TileCoords rookCoords(INVALID, move.from.y);
			TileCoords rookCastleCoords(INVALID, move.from.y);
			if (move.to.x == KING_LONG_CASTLE_X) {
				rookCoords.x = 0;
				rookCastleCoords.x = ROOK_LONG_CASTLE_X;
			}
			else {
				assert(move.to.x == KING_SHORT_CASTLE_X);
				rookCoords.x = 7;
				rookCastleCoords.x = ROOK_SHORT_CASTLE_X;
			}
			BoardTile rook = getTile(rookCoords);
			removePiece(rookCoords);
			assert(getTile(rookCastleCoords).type == EMPTY);
			setTile(rookCastleCoords, rook);
		}
	}
	else if (from.type == ROOK) {
		// We don't care to check if it's the first time or not. If the rook is moved from it's initial column, then we invalidate castling anyway
		if (move.from.x == 0) {
			if (move.from.y == 0) {
				m_positionInfo.canWhiteLongCastle = false;
			}
			else if (move.from.y == 7) {
				m_positionInfo.canBlackLongCastle = false;
			}
		}
		else if (move.from.x == 7) {
			if (move.from.y == 0) {
				m_positionInfo.canWhiteShortCastle = false;
			}
			else if (move.from.y == 7) {
				m_positionInfo.canBlackShortCastle = false;
			}
		}
	}
	
	// If someone captures a corner, remove castle rights
	if (move.to.x == 0) {
		if (move.to.y == 0) {
			m_positionInfo.canWhiteLongCastle = false;
		}
		else if (move.to.y == 7) {
			m_positionInfo.canBlackLongCastle = false;
		}
	}
	else if (move.to.x == 7) {
		if (move.to.y == 0) {
			m_positionInfo.canWhiteShortCastle = false;
		}
		else if (move.to.y == 7) {
			m_positionInfo.canBlackShortCastle = false;
		}
	}

	removePiece(move.from);
	setTile(move.to, from);

	m_positionInfo.nextPlayerColor = static_cast<Color>(~m_positionInfo.nextPlayerColor);
}

bool ChessBoard::isKingInCheck(Color color) const {
	MovesVector moves;
	TileCoords kingTile = findKing(color);
	getBishopMoves(color, kingTile.x, kingTile.y, moves);
	for (const BoardMove& m : moves) {
		BoardTile attackedTile = getTile(m.to);
		if (attackedTile.type == QUEEN || attackedTile.type == BISHOP) {
			return true;
		}
	}
	moves.clear();

	getRookMoves(color, kingTile.x, kingTile.y, moves);
	for (const BoardMove& m : moves) {
		BoardTile attackedtile = getTile(m.to);
		if (attackedtile.type == QUEEN || attackedtile.type == ROOK) {
			return true;
		}
	}
	moves.clear();

	getKnightMoves(color, kingTile.x, kingTile.y, moves);
	for (const BoardMove& m : moves) {
		BoardTile attackedtile = getTile(m.to);
		if (attackedtile.type == KNIGHT) {
			return true;
		}
	}
	moves.clear();

	getSurroundingMoves(color, kingTile.x, kingTile.y, moves);
	for (const BoardMove& m : moves) {
		BoardTile attackedTile = getTile(m.to);
		if (attackedTile.type == KING) {
			return true;
		}
	}

	int8_t dir = color == Color::WHITE ? 1 : -1;
	int8_t pawnY = kingTile.y + dir;
	if (pawnY >= 0 && pawnY < BOARD_SIZE) {
		TileCoords pawnCoords(INVALID, pawnY);
		for (int8_t i = -1; i <= 1; ++i) {
			pawnCoords.x = kingTile.x + i;
			if (i == 0 || pawnCoords.x < 0 || pawnCoords.x >= BOARD_SIZE) {
				continue;
			}

			const BoardTile pawnTile = getTile(pawnCoords);
			if (pawnTile.type == PAWN && pawnTile.color != color) {
				return true;
			}
		}
	}

	return false;
}

bool ChessBoard::isMoveValid(const BoardMove& move) const {
	const BoardTile fromTile = getTile(move.from);

	if (move.isCastle(fromTile.type)) {
		assert(fromTile.type == KING);
		if (isKingInCheck(fromTile.color)) { // cannot be in check at the current board before and during castling
			return false;
		}

		BoardMove inBetween(move.from, move.to);
		if (move.to.x == KING_LONG_CASTLE_X) {
			assert(fromTile.color == WHITE ? m_positionInfo.canWhiteLongCastle : m_positionInfo.canBlackLongCastle);
			inBetween.to.x = ROOK_LONG_CASTLE_X;
		}
		else {
			assert(move.to.x == KING_SHORT_CASTLE_X);
			assert(fromTile.color == WHITE ? m_positionInfo.canWhiteShortCastle : m_positionInfo.canBlackShortCastle);
			inBetween.to.x = ROOK_SHORT_CASTLE_X;
		}

		ChessBoard midPositionPlayed(*this);
		midPositionPlayed.playMove(inBetween);
		if (midPositionPlayed.isKingInCheck(fromTile.color)) {
			return false;
		}
	}

	ChessBoard movePlayedBoard(*this);
	movePlayedBoard.playMove(move);
	return !movePlayedBoard.isKingInCheck(fromTile.color);
}

bool ChessBoard::isDraw() const {
	for (uint8_t x = 0; x < BOARD_SIZE; ++x) {
		for (uint8_t y = 0; y < BOARD_SIZE; ++y) {
			const BoardTile tile = getTile(x, y);
			if (tile.type != EMPTY && tile.type != KING) {
				return false;
			}
		}
	}
	return true;
}

TileCoords ChessBoard::findKing(Color color) const {
	for (uint8_t x = 0; x < BOARD_SIZE; ++x) {
		for (uint8_t y = 0; y < BOARD_SIZE; ++y) {
			const BoardTile tile = getTile(x, y);
			if (tile.type == KING && tile.color == color) {
				return TileCoords(x, y);
			}
		}
	}
	assert(false); // There should always be a king of each color, otherwise the game should have ended
	return TileCoords();
}

void ChessBoard::getMovesForPiece(BoardTile tile, uint8_t x, uint8_t y, MovesVector& outMoves) const {
	switch (tile.type) {
	case PAWN: 		getPawnMoves(tile.color, x, y, outMoves); 	return;
	case ROOK: 		getRookMoves(tile.color, x, y, outMoves); 	return;
	case KNIGHT: 	getKnightMoves(tile.color, x, y, outMoves); return;
	case BISHOP: 	getBishopMoves(tile.color, x, y, outMoves); return;
	case QUEEN: 	getQueenMoves(tile.color, x, y, outMoves); 	return;
	case KING: 		getKingMoves(tile.color, x, y, outMoves); 	return;
	default: 		assert(false); 								return;
	}
}

void ChessBoard::getDirectionalMoves(Color color, int8_t sx, int8_t sy, int8_t dx, int8_t dy, MovesVector& outMoves) const {
	BoardMove move(sx, sy, sx, sy);

	for(uint8_t i = 0; i < BOARD_SIZE; ++i) { // using a for loop here only for the compiler
		move.to.x += dx;
		move.to.y += dy;
		if (move.to.areOutsideBoard()) {
			break;
		}

		const BoardTile toTile = getTile(move.to);
		if (toTile.type == EMPTY || toTile.color != color) {
			outMoves.push(move);
		}

		if (toTile.type != EMPTY) {
			break;
		}
	}
}

void ChessBoard::getSurroundingMoves(Color color, int8_t sx, int8_t sy, MovesVector& outMoves) const {
	BoardMove move(sx, sy, sx, sy);
	for (int8_t i = -1; i <= 1; ++i) {
		for (int8_t j = -1; j <= 1; ++j) {
			move.to.x = sx + i;
			move.to.y = sy + j;
			if ((i == 0 && j == 0) || move.to.areOutsideBoard()) {
				continue;
			}

			const BoardTile toTile = getTile(move.to);
			if (toTile.type == EMPTY || toTile.color != color) {
				outMoves.push(move);
			}
		}
	}
}

void ChessBoard::getBishopMoves(Color color, int8_t sx, int8_t sy, MovesVector& outMoves) const {
	for (int8_t i = -1; i <= 1; ++i) {
		if (i == 0) {
			continue;
		}

		for (int8_t j = -1; j <= 1; ++j) {
			if (j == 0) {
				continue;
			}
			getDirectionalMoves(color, sx, sy, i, j, outMoves);
		}
	}
}

void ChessBoard::getRookMoves(Color color, int8_t sx, int8_t sy, MovesVector& outMoves) const {
	for (int8_t i = -1; i <= 1; ++i) {
		if (i == 0) {
			continue;
		}
		getDirectionalMoves(color, sx, sy, i, 0, outMoves);
		getDirectionalMoves(color, sx, sy, 0, i, outMoves);
	}
}

void ChessBoard::getKnightMoves(Color color, int8_t sx, int8_t sy, MovesVector& outMoves) const {
	const BoardTile fromTile = getTile(sx, sy);
	BoardMove move(sx, sy, sx, sy);
	for (int8_t i = -1; i <= 1; ++i) {
		if (i == 0) {
			continue;
		}

		for (int8_t j = -1; j <= 1; ++j) {
			if (j == 0) {
				continue;
			}

			move.to.x = sx + 2 * i;
			move.to.y = sy + j;
			if (!move.to.areOutsideBoard()) {
				const BoardTile toTile = getTile(move.to);
				if (toTile.type == EMPTY || toTile.color != color) {
					outMoves.push(move);
				}
			}

			move.to.x = sx + j;
			move.to.y = sy + i * 2;
			if (!move.to.areOutsideBoard()) {
				const BoardTile toTile = getTile(move.to);
				if (toTile.type == EMPTY || toTile.color != color) {
					outMoves.push(move);
				}
			}
		}
	}
}

void ChessBoard::getQueenMoves(Color color, int8_t sx, int8_t sy, MovesVector& outMoves) const {
	getRookMoves(color, sx, sy, outMoves);
	getBishopMoves(color, sx, sy, outMoves);
}

void ChessBoard::getPawnMoves(Color color, int8_t sx, int8_t sy, MovesVector& outMoves) const {
	BoardMove move(sx, sy, sx, sy);
	const int8_t dir = color == WHITE ? 1 : -1;
	const int8_t enPas = color == WHITE ? 4 : 3;
	const int8_t prom = color == WHITE ? 7 : 0;
	const int8_t pawStart = color == WHITE ? 1 : 6;

	auto addPromotionMoves = [](const TileCoords& from, const TileCoords& to, MovesVector& outMoves) {
		BoardMove move(from, to);
		for (uint8_t p = 0; p < NUM_OF_TYPES; ++p) {
			if (p == QUEEN || p == KNIGHT || p == BISHOP || p == ROOK) {
				move.promotionType = static_cast<TileType>(p);
				outMoves.push(move);
			}
		}
	};

	move.to.y = sy + dir;
	assert(!move.to.areOutsideBoard()); // Pawn should always be on board on the y axis otehrwise there has been an error
	for (int8_t i = -1; i <= 1; ++i) {
		move.to.x = sx + i;
		if (move.to.x < 0 || move.to.x >= BOARD_SIZE) { // this can be improved a bit here using max and setting the for loop limits
			continue;
		}

		const BoardTile toTile = getTile(move.to);
		if ((i == 0 && toTile.type == EMPTY) || (i != 0 && toTile.type != EMPTY && toTile.color != color)) {
			if (move.to.y == prom) {
				addPromotionMoves(move.from, move.to, outMoves);
			}
			else {
				outMoves.push(move);
			}
		}
	}

	move.to.x = sx;
	move.to.y = sy + 2 * dir;
	if (sy == pawStart && getTile(move.to).type == EMPTY && getTile(sx, sy + dir).type == EMPTY) {
		assert(!move.to.areOutsideBoard()); // If the pawn was at the start then there should be enough space in from
		outMoves.push(move);
	}
	else if (sy == enPas) {
		move.to.y = sy + dir;
		move.enPassantPawn.y = sy;
		for (int8_t i = -1; i <= 1; ++i) {
			move.to.x = sx + i;
			if (i == 0 || move.to.x < 0 || move.to.x >= BOARD_SIZE) {
				continue;
			}
			const BoardTile enPasToTile = getTile(move.to);
			move.enPassantPawn.x = sx + i;
			const BoardTile enPasPawnTile = getTile(move.enPassantPawn);
			if (m_positionInfo.enPassantSquare == move.enPassantPawn && enPasPawnTile.color != color && enPasToTile.type == EMPTY) {
				assert(enPasPawnTile.type == PAWN); // must be pawn otherwise there has been an error
				outMoves.push(move);
				break; // if we find an en passant move we can go out, the other values won't have one
			}
		}
	}
}

void ChessBoard::getKingMoves(Color color, int8_t sx, int8_t sy, MovesVector& outMoves) const {
	getSurroundingMoves(color, sx, sy, outMoves);

	auto isSpaceBetweenEmpty = [this, &sy](int8_t start, int8_t end) {
		assert(start <= end);
		for (int8_t i = start; i <= end; ++i) {
			if (getTile(i, sy).type != EMPTY) {
				return false;
			}
		}
		return true;
	};

	const bool canLongCastle = color == WHITE ? m_positionInfo.canWhiteLongCastle : m_positionInfo.canBlackLongCastle;
	const bool canShortCastle = color == WHITE ? m_positionInfo.canWhiteShortCastle : m_positionInfo.canBlackShortCastle;

	BoardMove castle(sx, sy, sx, sy);
	if (canLongCastle && isSpaceBetweenEmpty(1, sx - 1)) {
		assert(getTile(0, sy).type == ROOK && getTile(0, sy).color == color);
		castle.to.x = KING_LONG_CASTLE_X;
		outMoves.push(castle);
	}

	if (canShortCastle && isSpaceBetweenEmpty(sx + 1, 6)) {
		assert(getTile(7, sy).type == ROOK && getTile(7, sy).color == color);
		castle.to.x = KING_SHORT_CASTLE_X;
		outMoves.push(castle);
	}
}
