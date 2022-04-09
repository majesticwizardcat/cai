#pragma once

class AIPlayer;

#include "game/player.h"
#include "tools/random-generator.h"
#include "ai/cai-population.h"

#include <nnpp.hpp>

typedef NNAi<float> AI;

class AIPlayer : public Player {
public:
	AIPlayer() = delete;
	AIPlayer(Color color, const AI* ai, uint cycles, uint cyclesPerMove)
		: Player(color), m_ai(ai), m_cycles(cycles), m_cyclesPerMove(cyclesPerMove), m_maxCycles(cycles) { }

	bool getMove(const ChessBoard& board, Move* outMove);
	inline int getCycles() const { return m_cycles; }

private:
	struct Position {
		const Move* move;
		float evaluation;
		NNPPStackVector<float> position;

		Position() = delete;
		Position(const Position& other) = delete;
		Position(const Move* move, float eval, NNPPStackVector<float> position)
			: move(move)
			, evaluation(eval)
			, position(std::move(position)) { }

		Position(Position&& other)
			: move(std::move(other.move))
			, evaluation(std::move(other.evaluation))
			, position(std::move(other.position)) { }

		bool operator<(const Position& other) const {
			return evaluation < other.evaluation;
		}

		Position& operator=(Position&& other) {
			move = std::move(other.move);
			evaluation = std::move(other.evaluation);
			position = std::move(other.position);
			return *this;
		}
	};

	const AI* m_ai;
	uint m_cycles;
	uint m_maxCycles;
	uint m_cyclesPerMove;
	RandomGenerator m_rgen;

	inline float fastLookup(const NNPPStackVector<float>& position) const {
		return m_ai->feedAt(FAST_LOOKUP_NETWORK_INDEX, position)[0];
	}

	inline uint cycles(float currentCycles, float currentMove, float randomBias) const {
		const NNPPStackVector<float> input = { currentCycles, currentMove, randomBias };
		return static_cast<uint>(std::abs(std::floor(m_ai->feedAt(CYCLES_MANAGER_NETWORK_INDEX, input)[0])));
	}

	inline float analyze(const NNPPStackVector<float>& position, uint cycles) const {
		NNPPStackVector<float> propagated = propagate(position, cycles);
		return m_ai->feedAt(ANALYZER_NETWORK_INDEX, propagated)[0];
	}

	inline NNPPStackVector<float> propagate(const NNPPStackVector<float>& position, uint cycles) const {
		NNPPStackVector<float> pos(position);
		for (int i = 0; i < cycles; ++i) {
			NNPPStackVector<float> out = m_ai->feedAt(PROPAGATOR_NETWORK_INDEX, pos);
			pos = std::move(out);
		}
		return pos;
	}

	inline uint calculateCyclesToUse(const ChessBoard& board, float randomBias) const {
		return m_cyclesPerMove > 0 ? m_cyclesPerMove
			: cycles(static_cast<float>(m_cycles), static_cast<float>(board.movesPlayed()), randomBias);
	}
};
