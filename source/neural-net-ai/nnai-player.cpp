#include "neural-net-ai/nnai-player.h"

#include <iostream>
#include <algorithm>
#include <tuple>
#include <vector>

MoveResult NNAIPlayer::getMove(const ChessBoard& board, BoardMove* outMove) {
	MovesVector moves;
	board.getMoves(m_color, moves);
	if (moves.empty()) {
		return MoveResult::OUT_OF_MOVES;
	}
	
	if (moves.size() == 1) {
		*outMove = std::move(moves[0]);
		return MoveResult::MOVE_OK;
	}

	std::vector<Position> nextPositions;
	nextPositions.reserve(moves.size());
	for (const auto& m : moves) {
		ChessBoard next(board);
		next.playMove(m);
		NNPPStackVector<float> values = next.asFloats();
		nextPositions.emplace_back(&m, 0.0f, std::move(values));
		Position* pos = &nextPositions.back();
		analyze(pos, 0);
		pos->evaluation *= m_rgen.get(0.975f, 1.0f); // Reduce to create some random moves more possible, encapsulates the "feeling" of the ai
	}

	uint cyclesPerPosition = std::max(1u, m_cyclesPerMove / static_cast<uint>(nextPositions.size()));
	while (nextPositions.size() > 1) {
		size_t rand = std::min(nextPositions.size() - 1, static_cast<size_t>(nextPositions.size() * m_rgen.get(0.0f, 1.0f)));
		analyze(&nextPositions[rand], cyclesPerPosition);
		auto worst = m_color == Color::WHITE ? std::min_element(nextPositions.begin(), nextPositions.end())
			: std::max_element(nextPositions.begin(), nextPositions.end());
		assert(!std::isnan(worst->evaluation));
		nextPositions.erase(worst);
	}
	*outMove = BoardMove(*nextPositions.back().move);
	return MoveResult::MOVE_OK;
}