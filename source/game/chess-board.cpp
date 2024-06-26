#include "game/chess-board.h"
#include "tools/board-hashing.hpp"

#include <iostream>

ChessBoard::ChessBoard()
		: m_positionData(0)
		, m_hash(0) {
	memset(m_tileData, 0, sizeof(m_boardTiles));
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
	
	calculateHashFromCurrentState();
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

	calculateHashFromCurrentState();
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

void ChessBoard::printMoveOnBoard(const BoardMove move) const {
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

void ChessBoard::getMoves(const Color color, MovesVector& outMoves) const {
	TileCoords kingCoords;
	for (uint8_t x = 0; x < BOARD_SIZE; ++x) {
		for (uint8_t y = 0; y < BOARD_SIZE; ++y) {
			const BoardTile tile = getTile(x, y);
			if (tile.type != EMPTY && tile.color == color) {
				if (tile.type == KING) {
					kingCoords = TileCoords(x, y);
				}
				getMovesForPiece(tile, x, y, outMoves);
			}
		}
	}

	assert(kingCoords.areValid() && getTile(kingCoords).type == KING && getTile(kingCoords).color == color); // We must have a king of the same color
	for (uint8_t i = 0; i < outMoves.size(); ++i) {
		if (!isMoveValid(outMoves[i], kingCoords)) {
			outMoves.erase(i);
			--i;
		}
	}
}

void ChessBoard::playMove(const BoardMove move) {
	assert(getTile(move.to).type != KING); // There should not be a move played that captures the king

// Although this does the same, remove piece will perform an extra check when debugging that's useful
// so we keep it like this, even though it's duplicated (this can be improved later)
	const auto removeAndUpdateHash = [this](const TileCoords coords) {
		m_hash ^= boardhashing::BOARD_HASH_TABLE.getBoardHashValue(getTile(coords), coords);
		removePiece(coords);
		m_hash ^= boardhashing::BOARD_HASH_TABLE.getBoardHashValue(BoardTile(0), coords);
	};

	const auto setAndUpdateHash = [this](const TileCoords coords, const BoardTile tile) {
		m_hash ^= boardhashing::BOARD_HASH_TABLE.getBoardHashValue(getTile(coords), coords);
		setTile(coords, tile);
		m_hash ^= boardhashing::BOARD_HASH_TABLE.getBoardHashValue(tile, coords);
	};

	const auto removeCastleAndUpdateHash = [this](const Color color, const bool isLongCastle, const bool curValue) {
		if (!curValue) {
			return;
		}
		m_hash ^= boardhashing::BOARD_HASH_TABLE.getCastleHashValue(true, color, isLongCastle);
		m_hash ^= boardhashing::BOARD_HASH_TABLE.getCastleHashValue(false, color, isLongCastle);
	};

	const auto removeCastlesCoords = [this, &removeCastleAndUpdateHash](const TileCoords coords) {
		if (coords.x == 0) {
			if (coords.y == 0) {
				removeCastleAndUpdateHash(WHITE, true, m_positionInfo.canWhiteLongCastle);
				m_positionInfo.canWhiteLongCastle = false;
			}
			else if (coords.y == 7) {
				removeCastleAndUpdateHash(BLACK, true, m_positionInfo.canBlackLongCastle);
				m_positionInfo.canBlackLongCastle = false;
			}
		}
		else if (coords.x == 7) {
			if (coords.y == 0) {
				removeCastleAndUpdateHash(WHITE, false, m_positionInfo.canWhiteShortCastle);
				m_positionInfo.canWhiteShortCastle = false;
			}
			else if (coords.y == 7) {
				removeCastleAndUpdateHash(BLACK, false, m_positionInfo.canBlackShortCastle);
				m_positionInfo.canBlackShortCastle = false;
			}
		}
	};

	const auto updatePlayerColorAndHash = [this]() {
		m_hash ^= boardhashing::BOARD_HASH_TABLE.getNextPlayerColorHashValue(m_positionInfo.nextPlayerColor);
		m_positionInfo.nextPlayerColor = static_cast<Color>(~m_positionInfo.nextPlayerColor);
		m_hash ^= boardhashing::BOARD_HASH_TABLE.getNextPlayerColorHashValue(m_positionInfo.nextPlayerColor);
	};

	BoardTile from = getTile(move.from);
	if (move.promotionType != EMPTY) {
		from.type = move.promotionType;
	}

	if (m_positionInfo.enPassantSquare.areValid())
	{
		m_hash ^= boardhashing::BOARD_HASH_TABLE.getEnPassantHashValue(m_positionInfo.enPassantSquare);
		m_positionInfo.enPassantSquare = TileCoords(INVALID, INVALID);
	}

	if (move.enPassantPawn.areValid()) {
		assert(from.type == PAWN && move.enPassantPawn.areValid() && getTile(move.enPassantPawn).type == PAWN);
		removeAndUpdateHash(move.enPassantPawn);
	}
	else if (from.type == PAWN && std::abs(move.from.y - move.to.y) == 2) {
		m_positionInfo.enPassantSquare = move.to;
		m_hash ^= boardhashing::BOARD_HASH_TABLE.getEnPassantHashValue(m_positionInfo.enPassantSquare);
	}
	else if (from.type == KING) {
		if (from.color == WHITE) {
			removeCastleAndUpdateHash(WHITE, true, m_positionInfo.canWhiteLongCastle);
			removeCastleAndUpdateHash(WHITE, false, m_positionInfo.canWhiteShortCastle);
			m_positionInfo.canWhiteLongCastle = false;
			m_positionInfo.canWhiteShortCastle = false;
		}
		else {
			removeCastleAndUpdateHash(BLACK, true, m_positionInfo.canBlackLongCastle);
			removeCastleAndUpdateHash(BLACK, false, m_positionInfo.canBlackShortCastle);
			m_positionInfo.canBlackLongCastle = false;
			m_positionInfo.canBlackShortCastle = false;
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
			removeAndUpdateHash(rookCoords);
			assert(getTile(rookCastleCoords).type == EMPTY);
			setAndUpdateHash(rookCastleCoords, BoardTile(from.color, ROOK));
		}
	}

	if (from.type == ROOK) {
		removeCastlesCoords(move.from);
	}
	
	// If someone captures a corner, remove castle rights
	removeCastlesCoords(move.to);

	removeAndUpdateHash(move.from);
	setAndUpdateHash(move.to, from);
	updatePlayerColorAndHash();
}

void ChessBoard::calculateHashFromCurrentState() {
	m_hash = 0;
	m_hash ^= boardhashing::BOARD_HASH_TABLE.getNextPlayerColorHashValue(m_positionInfo.nextPlayerColor);
	m_hash ^= boardhashing::BOARD_HASH_TABLE.getCastleHashValue(m_positionInfo.canWhiteLongCastle, WHITE, true);
	m_hash ^= boardhashing::BOARD_HASH_TABLE.getCastleHashValue(m_positionInfo.canWhiteShortCastle, WHITE, false);
	m_hash ^= boardhashing::BOARD_HASH_TABLE.getCastleHashValue(m_positionInfo.canBlackLongCastle, BLACK, true);
	m_hash ^= boardhashing::BOARD_HASH_TABLE.getCastleHashValue(m_positionInfo.canBlackShortCastle, BLACK, false);

	if (m_positionInfo.enPassantSquare.areValid()) {
		m_hash ^= boardhashing::BOARD_HASH_TABLE.getEnPassantHashValue(m_positionInfo.enPassantSquare);
	}

	for (uint8_t x = 0; x < BOARD_SIZE; ++x) {
		for (uint8_t y = 0; y < BOARD_SIZE; ++y) {
			TileCoords coords(x, y);
			m_hash ^= boardhashing::BOARD_HASH_TABLE.getBoardHashValue(getTile(coords), coords);
		}
	}
}

bool ChessBoard::isAttacked(const Color color, const TileCoords coords) const {
	MovesVector moves;
	getQueenMoves(color, coords.x, coords.y, moves);

	const bool isWhite = color == WHITE;
	for (const auto& m : moves) {
		const TileType attackerType = getTile(m.to).type;
		if (attackerType == QUEEN) {
			return true;
		}

		const int8_t absDeltaX = std::abs(coords.x - m.to.x);
		const int8_t absDeltaY = std::abs(coords.y - m.to.y);
		const bool isDiag = absDeltaX == absDeltaY;
		if ((isDiag && attackerType == BISHOP) || (!isDiag && attackerType == ROOK)) {
			return true;
		}

		const bool isNeighbour = absDeltaX <= 1 && absDeltaY <= 1;
		const bool isAttackedByPawn  = isDiag && isNeighbour && ((isWhite && m.to.y > coords.y) || (!isWhite && m.to.y < coords.y));
		if ((isNeighbour && attackerType == KING) || (isAttackedByPawn && attackerType == PAWN)) {
			return true;
		}
	}

	moves.clear();
	getKnightMoves(color, coords.x, coords.y, moves);
	for (const auto& m : moves) {
		if (getTile(m.to).type == KNIGHT) {
			return true;
		}
	}

	return false;
}

bool ChessBoard::isMoveValid(const BoardMove move, const TileCoords kingCoords) const {
	const BoardTile fromTile = getTile(move.from);

	if (move.isCastle(fromTile.type)) {
		assert(fromTile.type == KING);
		if (isAttacked(fromTile.color, kingCoords)) { // cannot be in check at the current board before and during castling
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
		midPositionPlayed.updateBoardDataFromMove(inBetween, fromTile);
		assert(midPositionPlayed.getTile(inBetween.to).type == KING);
		if (midPositionPlayed.isAttacked(fromTile.color, inBetween.to)) {
			return false;
		}
	}

	ChessBoard movePlayedBoard(*this);
	movePlayedBoard.updateBoardDataFromMove(move, fromTile);
	if (fromTile.type == KING) {
		assert(movePlayedBoard.getTile(move.to).type == KING);
		return !movePlayedBoard.isAttacked(fromTile.color, move.to); // the king moved to the new position
	}

	assert(movePlayedBoard.getTile(kingCoords).type == KING);
	return !movePlayedBoard.isAttacked(fromTile.color, kingCoords); // otherwise the king remained where he was
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

void ChessBoard::getMovesForPiece(const BoardTile tile, const uint8_t x, const uint8_t y, MovesVector& outMoves) const {
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

void ChessBoard::getDirectionalMoves(const Color color, const int8_t sx, const int8_t sy, const int8_t dx, const int8_t dy, MovesVector& outMoves) const {
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

void ChessBoard::getSurroundingMoves(const Color color, const int8_t sx, const int8_t sy, MovesVector& outMoves) const {
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

void ChessBoard::getBishopMoves(const Color color, const int8_t sx, const int8_t sy, MovesVector& outMoves) const {
	getDirectionalMoves(color, sx, sy, -1, -1, outMoves);
	getDirectionalMoves(color, sx, sy, -1, 1, outMoves);
	getDirectionalMoves(color, sx, sy, 1, -1, outMoves);
	getDirectionalMoves(color, sx, sy, 1, 1, outMoves);
}

void ChessBoard::getRookMoves(const Color color, const int8_t sx, const int8_t sy, MovesVector& outMoves) const {
	getDirectionalMoves(color, sx, sy, -1, 0, outMoves);
	getDirectionalMoves(color, sx, sy, 1, 0, outMoves);
	getDirectionalMoves(color, sx, sy, 0, -1, outMoves);
	getDirectionalMoves(color, sx, sy, 0, 1, outMoves);
}

void ChessBoard::getKnightMoves(const Color color, const int8_t sx, const int8_t sy, MovesVector& outMoves) const {
	const auto addIfEmptyOrOpositeColor = [this, color, sx, sy, &outMoves](const int8_t toX, const int8_t toY) {
		const BoardMove move(sx, sy, toX, toY);
		if (!move.to.areOutsideBoard()) {
			const BoardTile toTile = getTile(move.to);
			if (toTile.type == EMPTY || toTile.color != color) {
				outMoves.push(move);
			}
		}
	};

	addIfEmptyOrOpositeColor(sx - 2, sy - 1);
	addIfEmptyOrOpositeColor(sx - 1, sy - 2);
	addIfEmptyOrOpositeColor(sx - 2, sy + 1);
	addIfEmptyOrOpositeColor(sx + 1, sy - 2);
	addIfEmptyOrOpositeColor(sx + 2, sy - 1);
	addIfEmptyOrOpositeColor(sx - 1, sy + 2);
	addIfEmptyOrOpositeColor(sx + 2, sy + 1);
	addIfEmptyOrOpositeColor(sx + 1, sy + 2);
}

void ChessBoard::getQueenMoves(const Color color, const int8_t sx, const int8_t sy, MovesVector& outMoves) const {
	getRookMoves(color, sx, sy, outMoves);
	getBishopMoves(color, sx, sy, outMoves);
}

void ChessBoard::getPawnMoves(const Color color, const int8_t sx, const int8_t sy, MovesVector& outMoves) const {
	const int8_t dir = color == WHITE ? 1 : -1;
	const int8_t enPas = color == WHITE ? 4 : 3;
	const int8_t prom = color == WHITE ? 7 : 0;
	const int8_t pawStart = color == WHITE ? 1 : 6;

	constexpr auto addPromotionMoves = [](const TileCoords from, const TileCoords to, MovesVector& outMoves) {
		BoardMove move(from, to);
		for (uint8_t p = 0; p < NUM_OF_TYPES; ++p) {
			if (p == QUEEN || p == KNIGHT || p == BISHOP || p == ROOK) {
				move.promotionType = static_cast<TileType>(p);
				outMoves.push(move);
			}
		}
	};

	BoardMove move(sx, sy, sx, sy + dir);
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

void ChessBoard::getKingMoves(const Color color, const int8_t sx, const int8_t sy, MovesVector& outMoves) const {
	getSurroundingMoves(color, sx, sy, outMoves);

	const auto isSpaceBetweenEmpty = [this, &sy](const int8_t start, const int8_t end) {
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

void ChessBoard::updateBoardDataFromMove(const BoardMove move, BoardTile fromTile) {
	assert(getTile(move.to).type != KING); // There should not be a move played that captures the king
	if (move.promotionType != EMPTY) {
		fromTile.type = move.promotionType;
	}

	if (move.enPassantPawn.areValid()) {
		assert(fromTile.type == PAWN && move.enPassantPawn.areValid() && getTile(move.enPassantPawn).type == PAWN);
		removePiece(move.enPassantPawn);
	}
	else if (fromTile.type == KING) {
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

			removePiece(rookCoords);
			assert(getTile(rookCastleCoords).type == EMPTY);
			setTile(rookCastleCoords, BoardTile(fromTile.color, ROOK));
		}
	}
	
	removePiece(move.from);
	setTile(move.to, fromTile);
}
