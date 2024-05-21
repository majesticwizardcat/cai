#include "neural-net-ai/nnai-player.h"

#include <iostream>
#include <limits>

MoveResult NNAIPlayer::getMove(const ChessBoard& board, BoardMove* outMove) {
	MovesVector moves;
	board.getMoves(m_color, moves);
	if (moves.empty()) {
		return MoveResult::OUT_OF_MOVES;
	}

	float bestEval = m_color == WHITE ? std::numeric_limits<float>::min() : std::numeric_limits<float>::max();
	uint32_t bestEvalIndex = 0;
	for (uint32_t i = 0; i < moves.size(); ++i) {
		ChessBoard next(board);
		next.playMove(moves[i]);
		const nnpp::NNPPStackVector<float> values = next.asFloats();
		const float eval = analyze(values);
		if ((m_color == WHITE && eval > bestEval) || (m_color != WHITE && eval < bestEval)) {
			bestEval = eval;
			bestEvalIndex = i;
		}
	}

	*outMove = moves[bestEvalIndex];
	return MoveResult::MOVE_OK;
}
