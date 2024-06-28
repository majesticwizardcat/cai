#pragma once

class NNAIPlayer;

#include "game/player.h"
#include "tools/random-generator.h"
#include "neural-net-ai/cai-population.h"

#include <nnpp.hpp>

typedef nnpp::NNAi<float> NNAI;

const float REDUCTION = 1000.0f;

class NNAIPlayer : public Player {
public:
	NNAIPlayer() = delete;
	NNAIPlayer(Color color, const NNAI* ai, nnpp::NeuronBuffer<float>* const neuronBuffer)
			: Player(color)
			, m_ai(ai)
			, m_neuronBuffer(neuronBuffer) {
		assert(neuronBuffer);
	}

	MoveResult getMove(const ChessBoard& board, BoardMove* outMove) override final;

private:
	const NNAI* m_ai;
	RandomGenerator m_rgen;
	nnpp::NeuronBuffer<float>* const m_neuronBuffer;

	inline void reduce(nnpp::NNPPStackVector<float>& vec) const {
		for (uint i = 0; i < vec.size(); ++i) {
			while (std::abs(vec[i]) > REDUCTION) {
				vec[i] *= 0.1f;
			}
		}
	}

	constexpr float analyze(const nnpp::NNPPStackVector<float>& position) const {
		return m_ai->feedAt(ANALYZER_NETWORK_INDEX, position, *m_neuronBuffer)[0];
	}
};
