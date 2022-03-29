#include "ai/ai-player.h"

#include <iostream>
#include <algorithm>
#include <tuple>
#include <vector>

bool AIPlayer::getMove(const Board& board, Move* outMove) {
	std::vector<Move> moves = board.getMoves(m_color);
	if (moves.empty()) {
		return false;
	}
	
	if (moves.size() == 1) {
		*outMove = moves.back();
		return true;
	}

	std::vector<Position> nextPositions;
	nextPositions.reserve(moves.size());
	for (const Move& m : moves) {
		Board next(board);
		next.playMove(m);
		std::vector<float> values = next.asFloats();
		float eval = fastLookup(values);
		nextPositions.emplace_back(&m, eval, std::move(values));
	}
	std::sort(nextPositions.begin(), nextPositions.end());

	uint cyclesToUse = calculateCyclesToUse(board, m_rgen.get(0.0f, 1.0f));
	if (m_maxCycles > 0) {
		if (m_cycles == 0) {
			return false;
		}

		cyclesToUse = std::min(m_cycles, cyclesToUse);
		m_cycles -= cyclesToUse;
	}

	uint cyclesPerPosition = std::max(1u, cyclesToUse / static_cast<uint>(nextPositions.size()));
	const size_t lastIndex = nextPositions.size() - 1;
	for (uint i = 0; i < cyclesToUse; i += cyclesPerPosition) {
		assert(std::is_sorted(nextPositions.begin(), nextPositions.end()));
		Position& currentBest = nextPositions.back();
		currentBest.evaluation = analyze(currentBest.position, cyclesPerPosition);
		size_t newIndex = lastIndex - 1;
		while (newIndex > 0 && currentBest < nextPositions[newIndex--]) { }
		if (currentBest < nextPositions[newIndex]) {
			std::iter_swap(nextPositions.begin() + lastIndex, nextPositions.begin() + newIndex);
		}
	}
	*outMove = *(nextPositions.back().move);
	return true;
}
