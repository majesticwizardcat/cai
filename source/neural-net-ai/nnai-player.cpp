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
	uint bestEvalIndex = 0;
	for (uint i = 0; i < moves.size(); ++i) {
		ChessBoard next(board);
		next.playMove(moves[i]);
		NNPPStackVector<float> values = next.asFloats();
		float eval = analyze(values);
		eval *= m_rgen.get(0.975f, 1.025f); // Reduce to create some random moves more possible, encapsulates the "feeling" of the ai
		if ((m_color == WHITE && eval > bestEval) || (m_color != WHITE && eval < bestEval)) {
			bestEval = eval;
			bestEvalIndex = i;
		}
	}

	*outMove = moves[bestEvalIndex];
	return MoveResult::MOVE_OK;
}
