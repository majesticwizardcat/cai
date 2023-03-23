#pragma once

#include "game/chess-board.h"

typedef int64_t ChessBoardEvalType;

// Max evaluation is 9 queens (8 promoted pawns + 1 queen), 2 rooks, 2 bishops, 2 knights
// Total points = +- (9 * 10 + 2 * 5 + 2 * 3 + 2 * 2) * 10 = +-1100
// Kings aren't counted because they always cancel each other out, otherwise the game is over
// MAX and MIN evaluations are used to denote win
static const ChessBoardEvalType CHESS_BOARD_MAX_EVALUATION = 10000;
static const ChessBoardEvalType CHESS_BOARD_MIN_EVALUATION = -10000;

class ChessBoardEvaluator {
public: 
	inline ChessBoardEvalType operator()(const ChessBoard& board, const MovesVector& availableMoves) const {
		const int16_t currentPlayerColorValue = board.getNextPlayerColor() == WHITE ? 1 : -1;

		if (availableMoves.empty()) {
			// It is checkmate
			if (board.isKingInCheck(board.getNextPlayerColor())) {
				return currentPlayerColorValue * CHESS_BOARD_MAX_EVALUATION;
			}
			// it is stalemate
			return 0;
		}

		auto getEvalForPiece = [](const BoardTile& tile, uint8_t x, uint8_t y) {
			ChessBoardEvalType eval = 0;
			switch (tile.type)
			{
			case PAWN:		eval += 10;  break;
			case KNIGHT: 	eval += 30;  break;
			case BISHOP: 	eval += 35;  break;
			case ROOK: 		eval += 50;  break;
			case QUEEN: 	eval += 100; break;
			default: 		break;
			}
			return eval;
		};

		ChessBoardEvalType evaluation = 0;
		for (uint8_t x = 0; x < BOARD_SIZE; ++x) {
			for (uint8_t y = 0; y < BOARD_SIZE; ++y) {
				const BoardTile tile = board.getTile(x, y);
				const int16_t colorValue = tile.color == WHITE ? 1 : -1;
				ChessBoardEvalType eval = getEvalForPiece(tile, x, y);
				evaluation += colorValue * eval;
			}
		}
		return evaluation;
	}
};