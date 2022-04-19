#pragma once

class AIPlayer;

#include "game/player.h"
#include "tools/random-generator.h"
#include "ai/cai-population.h"

#include <nnpp.hpp>

typedef NNAi<float> AI;

const float REDUCTION = 1000.0f;

class AIPlayer : public Player {
public:
	AIPlayer() = delete;
	AIPlayer(Color color, const AI* ai, uint cycles, uint cyclesPerMove)
		: Player(color), m_ai(ai), m_cycles(cycles), m_cyclesPerMove(cyclesPerMove), m_maxCycles(cycles) { }

	MoveResult getMove(const ChessBoard& board, Move* outMove);
	void revert();

private:
	struct Position {
		const Move* move;
		float evaluation;
		NNPPStackVector<float> board;

		Position() = delete;
		Position(const Position& other) = delete;
		Position(const Move* move, float eval, NNPPStackVector<float> board)
			: move(move)
			, evaluation(eval)
			, board(std::move(board)) { }

		Position(Position&& other)
			: move(std::move(other.move))
			, evaluation(std::move(other.evaluation))
			, board(std::move(other.board)) { }

		bool operator<(const Position& other) const {
			return evaluation < other.evaluation;
		}

		Position& operator=(Position&& other) {
			move = std::move(other.move);
			evaluation = std::move(other.evaluation);
			board = std::move(other.board);
			return *this;
		}
	};

	struct AIState {
		uint cycles;

		AIState(uint cycles)
			: cycles(cycles) { }
	};

	const AI* m_ai;
	uint m_cycles;
	uint m_maxCycles;
	uint m_cyclesPerMove;
	std::vector<AIState> m_statesStack;
	RandomGenerator m_rgen;

	inline void reduce(NNPPStackVector<float>& vec) const {
		for (uint i = 0; i < vec.size(); ++i) {
			while (std::abs(vec[i]) > REDUCTION) {
				vec[i] *= 0.1f;
			}
		}
	}

	inline float fastLookup(const NNPPStackVector<float>& board) const {
		NNPPStackVector<float> res = m_ai->feedAt(FAST_LOOKUP_NETWORK_INDEX, board);
		reduce(res);
		return res[0];
	}

	inline uint cycles(float currentCycles, float currentMove, float randomBias) const {
		const NNPPStackVector<float> input = { currentCycles, currentMove, randomBias };
		return static_cast<uint>(std::abs(std::floor(m_ai->feedAt(CYCLES_MANAGER_NETWORK_INDEX, input)[0])));
	}

	inline void analyze(Position* position, uint cycles) const {
		propagate(position, cycles);
		NNPPStackVector<float> res = m_ai->feedAt(ANALYZER_NETWORK_INDEX, position->board);
		reduce(res);
		position->evaluation = res[0];
	}

	inline void propagate(Position* position, uint cycles) const {
		for (int i = 0; i < cycles; ++i) {
			NNPPStackVector<float> out = m_ai->feedAt(PROPAGATOR_NETWORK_INDEX, position->board);
			reduce(out);
			position->board = std::move(out);
		}
	}

	inline uint calculateCyclesToUse(const ChessBoard& board, float randomBias) const {
		return m_cyclesPerMove > 0 ? m_cyclesPerMove
			: cycles(static_cast<float>(m_cycles), static_cast<float>(board.movesPlayed()), randomBias);
	}
	
	inline void pushNewState() {
		m_statesStack.emplace_back(m_cycles);
	}
	
	inline void applyState(const AIState& state) {
		m_cycles = state.cycles;
	}
};
