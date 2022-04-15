#include "ai/ai-player.h"

#include <iostream>
#include <algorithm>
#include <tuple>
#include <vector>

bool AIPlayer::getMove(const ChessBoard& board, Move* outMove) {
	MovesStackVector moves;
	board.getMoves(m_color, &moves);
	if (moves.empty()) {
		return false;
	}
	
	if (moves.size() == 1) {
		*outMove = std::move(moves[0]);
		return true;
	}

	std::vector<Position> nextPositions;
	nextPositions.reserve(moves.size());
	for (const auto& m : moves) {
		Board next(board);
		next.playMove(m);
		NNPPStackVector<float> values = next.asFloats();
		float eval = fastLookup(values);
		nextPositions.emplace_back(&m, eval, std::move(values));
	}

	uint cyclesToUse = calculateCyclesToUse(board, m_rgen.get(0.0f, 1.0f));
	if (m_maxCycles > 0) {
		if (m_cycles == 0) {
			return false;
		}
		cyclesToUse = std::max(static_cast<uint>(nextPositions.size()), cyclesToUse);
		if (m_cycles < cyclesToUse) {
			m_cycles = 0;
		}
		else {
			m_cycles -= cyclesToUse;
		}
		assert(m_maxCycles > m_cycles);
	}

	uint cyclesPerPosition = std::max(1u, cyclesToUse / static_cast<uint>(nextPositions.size()));
	while (nextPositions.size() > 1) {
		size_t rand = std::min(nextPositions.size() - 1, static_cast<size_t>(nextPositions.size() * m_rgen.get(0.0f, 1.0f)));
		analyze(&nextPositions[rand], cyclesPerPosition);
		auto worst = m_color == Color::WHITE ? std::min_element(nextPositions.begin(), nextPositions.end())
			: std::max_element(nextPositions.begin(), nextPositions.end());
		assert(!std::isnan(worst->evaluation));
		nextPositions.erase(worst);
	}
	*outMove = Move(*nextPositions.back().move);
	return true;
}
