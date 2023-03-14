#pragma once

class MinMaxAiPlayer;

#include "game/player.h"
#include "min-max-ai/min-max-tree.h"
#include "min-max-ai/chess-board-evaluator.hpp"
#include "tools/random-generator.h"

#include <mutex>

typedef MinMaxTree<ChessBoardEvalType, ChessBoardEvaluator, CHESS_BOARD_MIN_EVALUATION, CHESS_BOARD_MAX_EVALUATION> MinMaxTreeChessBoard;

class MinMaxAiPlayer : public Player {
public:
	MinMaxAiPlayer(Color color, bool printEval, bool randomPadding)
		: Player(color)
		, m_printEval(printEval)
		, m_useRandomPadding(randomPadding) { }

	MoveResult getMove(const ChessBoard& board, BoardMove* move);

private:
	bool m_printEval;
	bool m_useRandomPadding;
	RandomGenerator m_rgen;
	std::mutex m_evalMutex;
	std::atomic<uint8_t> m_nextMoveIndexAtomic;
};
