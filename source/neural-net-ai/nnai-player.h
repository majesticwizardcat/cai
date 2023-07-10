#pragma once

class NNAIPlayer;

#include "game/player.h"
#include "tools/random-generator.h"
#include "neural-net-ai/cai-population.h"

#include <nnpp.hpp>

typedef NNAi<float> NNAI;

const float REDUCTION = 1000.0f;

class NNAIPlayer : public Player {
public:
	NNAIPlayer() = delete;
	NNAIPlayer(Color color, const NNAI* ai, uint cyclesPerMove, NeuronBuffer<float>* const neuronBuffer)
			: Player(color)
			, m_ai(ai)
			, m_cyclesPerMove(cyclesPerMove)
			, m_neuronBuffer(neuronBuffer) {
		assert(neuronBuffer);
	}

	MoveResult getMove(const ChessBoard& board, BoardMove* outMove);

private:
	struct Position {
		const BoardMove* move;
		float evaluation;
		NNPPStackVector<float> board;

		Position() = delete;
		Position(const Position& other) = delete;
		Position(const BoardMove* move, float eval, NNPPStackVector<float> board)
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

	const NNAI* m_ai;
	uint m_cyclesPerMove;
	RandomGenerator m_rgen;
	NeuronBuffer<float>* const m_neuronBuffer;

	inline void reduce(NNPPStackVector<float>& vec) const {
		for (uint i = 0; i < vec.size(); ++i) {
			while (std::abs(vec[i]) > REDUCTION) {
				vec[i] *= 0.1f;
			}
		}
	}

	inline void analyze(Position* position, uint cycles) const {
		propagate(position, cycles);
		NNPPStackVector<float> res = m_ai->feedAt(ANALYZER_NETWORK_INDEX, position->board, *m_neuronBuffer);
		reduce(res);
		position->evaluation = res[0];
	}

	inline void propagate(Position* position, uint cycles) const {
		for (int i = 0; i < cycles; ++i) {
			NNPPStackVector<float> out = m_ai->feedAt(PROPAGATOR_NETWORK_INDEX, position->board, *m_neuronBuffer);
			reduce(out);
			position->board = std::move(out);
		}
	}
};
