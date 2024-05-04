#pragma once

#include "game/chess-board.h"

// Max evaluation is 9 queens (8 promoted pawns + 1 queen), 2 rooks, 2 bishops, 2 knights
// Total points = +- (9 * 10 + 2 * 5 + 2 * 3 + 2 * 2) * 10 = +-1100
// Kings aren't counted because they always cancel each other out, otherwise the game is over
// MAX and MIN evaluations are used to denote win
static const int16_t CHESS_BOARD_MAX_EVALUATION = 10000;
static const int16_t CHESS_BOARD_MIN_EVALUATION = -10000;

inline int16_t evaluate(const ChessBoard& board, const MovesVector& availableMoves) {
	if (availableMoves.empty()) {
		// It is checkmate
		if (board.isKingInCheck(board.getNextPlayerColor())) {
			return board.getNextPlayerColor() == WHITE ? CHESS_BOARD_MIN_EVALUATION : CHESS_BOARD_MAX_EVALUATION;
		}
		// it is stalemate
		return 0;
	}

	auto getEvalForPiece = [](const BoardTile& tile) {
		switch (tile.type)
		{
		case PAWN:		return 10;
		case KNIGHT: 	return 30;
		case BISHOP: 	return 35;
		case ROOK: 		return 50;
		case QUEEN: 	return 100;
		default: 		return 0;
		}
	};

	int16_t evaluation = 0;
	for (uint8_t x = 0; x < BOARD_SIZE; ++x) {
		for (uint8_t y = 0; y < BOARD_SIZE; ++y) {
			const BoardTile tile = board.getTile(x, y);
			const int16_t colorValue = tile.color == WHITE ? 1 : -1;
			evaluation += colorValue * getEvalForPiece(tile);
		}
	}
	return evaluation;
}