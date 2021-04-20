#include "ai/ai-player.h"

#include <iostream>
#include <algorithm>
#include <tuple>

bool AIPlayer::getMove(const Board& board, Move* move) {
	std::vector<std::tuple<Move, float, std::vector<float>>> nextPositions;
	std::vector<Move> moves = board.getMoves(m_color);
	if (moves.size() == 0) {
		return false;
	}
	nextPositions.reserve(moves.size());
	for (const auto& m : moves) {
		Board next(board);
		next.playMove(m);
		std::vector<float> values = next.asFloats();
		float val = m_ai->fastLookup(values);
		nextPositions.push_back(std::make_tuple(m, m_ai->fastLookup(values) + m_rgen.get(0.0f, 0.1f), std::move(next.asFloats())));
	}
	int cyclesToUse = m_cyclesPerMove > 0 ? m_cyclesPerMove
		: m_ai->cycles((float) m_cycles / (float) m_maxCycles, (float) board.movesPlayed() * 0.01f) * 0.7f * m_cycles;
	auto cmp = [&](const std::tuple<const Move&, float, std::vector<float>>& p0,
				 const std::tuple<const Move&, float, std::vector<float>>& p1) {
		return m_color == Color::WHITE ? std::get<1>(p0) > std::get<1>(p1)
			: std::get<1>(p0) < std::get<1>(p1);
	};
	std::sort(nextPositions.begin(), nextPositions.end(), cmp);
	*move = std::get<0>(*(nextPositions.begin()));
	float per = (float) ((float) (nextPositions.size() - 1) / (float) (nextPositions.size()));
	float bestValue = m_ai->analyze(std::get<2>(*(nextPositions.begin())), per * cyclesToUse)
		* std::get<1>(*(nextPositions.begin()));
	for (int i = 1; i < nextPositions.size(); ++i) {
		per = (float) ((float) (nextPositions.size() - i - 1) / (float) (nextPositions.size()));
		float val = m_ai->analyze(std::get<2>(nextPositions[i]), per * cyclesToUse);
		float value = val * std::get<1>(nextPositions[i]);
		if ((m_color == Color::WHITE && value > bestValue)
			|| (m_color == Color::BLACK && value < bestValue)) {
			bestValue = value;
			*move = std::get<0>(nextPositions[i]);
		}
	}
	if (m_useCycles) {
		m_cycles -= cyclesToUse;
		if (m_cycles <= 0) {
			return false;
		}
	}
	return true;
}

