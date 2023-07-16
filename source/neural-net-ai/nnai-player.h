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
	NNAIPlayer(Color color, const NNAI* ai, NeuronBuffer<float>* const neuronBuffer)
			: Player(color)
			, m_ai(ai)
			, m_neuronBuffer(neuronBuffer) {
		assert(neuronBuffer);
	}

	MoveResult getMove(const ChessBoard& board, BoardMove* outMove);

private:
	const NNAI* m_ai;
	RandomGenerator m_rgen;
	NeuronBuffer<float>* const m_neuronBuffer;

	inline void reduce(NNPPStackVector<float>& vec) const {
		for (uint i = 0; i < vec.size(); ++i) {
			while (std::abs(vec[i]) > REDUCTION) {
				vec[i] *= 0.1f;
			}
		}
	}

	inline float analyze(NNPPStackVector<float>& position) const {
		NNPPStackVector<float> res = m_ai->feedAt(ANALYZER_NETWORK_INDEX, position, *m_neuronBuffer);
//		reduce(res);
		return res[0];
	}
};
