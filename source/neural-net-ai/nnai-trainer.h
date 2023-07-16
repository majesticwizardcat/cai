#pragma once

class AITrainer;

#include "neural-net-ai/cai-population.h"
#include "neural-net-ai/nnai-player.h"
#include "game/game.h"

#include <nnpp.hpp>

#include <tuple>
#include <unordered_set>
#include <mutex>
#include <random>
#include <algorithm>

const float POINTS_PER_GAME = 25.0f;
const float DRAW_POINTS = 3.5f;
const float RANDOM_WIN_POINTS = 7.0f;
const float MAX_MUTATION_CHANCE = 0.2f;
const float MUTATION_FREQ_CHANGE = 0.1f;
const float LAYER_ADDITION_CHANCE = 1.0f;
const float MAX_LAYER_MUTATION_CHANCE = 0.1f;
const float MIN_MUTATION_VALUE = MIN_AI_WEIGHT_VALUE * 15.0f;
const float MAX_MUTATION_VALUE = MAX_AI_WEIGHT_VALUE * 15.0f;
const uint MAX_LAYER_MUTATION = 2;
const uint GAMES_PER_POP = 20;
const uint TRAINING_SESSIONS_REQUIRED = GAMES_PER_POP;
const float CHILD_REGRESSION = 0.8f;
const uint MAX_MOVES_PER_GAME = 160;
const uint MAX_MOVES_PER_RANDOM_GAME = 80;

struct TrainTest {
public:
	uint cyclesPerMove;
	float points;

	TrainTest() {}
	TrainTest(uint cyclesPerMove, float points)
		: cyclesPerMove(cyclesPerMove)
		, points(points) { }
	
	bool operator<(const TrainTest& other) const {
		return cyclesPerMove < other.cyclesPerMove;
	}
};

class NNAITrainer : public NNPPTrainer<float> {
public:
	NNAITrainer() = delete;
	NNAITrainer(const AITrainer& other) = delete;
	NNAITrainer(uint sessions, uint threads, CAIPopulation* const population) 
		: NNPPTrainer<float>(sessions, threads, population, getDefaultEvolutionInfoFloat())
		, m_indexDist(0, m_trainee->getPopulationSize() - 1)
		, m_realDist(0.0f, 1.0f) { }

protected:
	std::vector<NNPPTrainingUpdate<float>> runSession(NeuronBuffer<float>& neuronBuffer) override;
	uint sessionsTillEvolution() const override;
	float getAvgScoreImportance() const override { return 0.5f; }

	void setEvolutionInfo(EvolutionInfo<float>& evolutionInfo) const override {
		evolutionInfo.minMutationValue = MIN_MUTATION_VALUE;
		evolutionInfo.maxMutationValue = MAX_MUTATION_VALUE;
		evolutionInfo.weightMutationChance = getWeightMutationChance();
		evolutionInfo.maxLayersMutation = MAX_LAYER_MUTATION;
		evolutionInfo.layerAdditionChance = LAYER_ADDITION_CHANCE;
		evolutionInfo.layerMutationChance = getLayerMutationChance();
		evolutionInfo.childRegressionPercentage = CHILD_REGRESSION;
		evolutionInfo.minTrainingSessionsRequired = TRAINING_SESSIONS_REQUIRED;
	}

private:
	std::random_device m_randomDevice;
	std::uniform_real_distribution<float> m_realDist;
	std::uniform_int_distribution<uint> m_indexDist;
	std::unordered_set<uint> m_occupied;
	std::mutex m_occupiedSetLock;

	inline static float calculatePoints(float score0, float score1) {
		const float alpha = 0.007f;
		float absDif = std::abs(score0 - score1);
		float mul = absDif >= 300.0f ? 0.1f : std::sqrt(1.0f / std::exp(absDif * alpha));
		assert(mul >= 0.1f);
		return mul * POINTS_PER_GAME;
	}

	inline float getWeightMutationChance() const {
		return std::abs(std::cos(m_trainee->getGenerartion() * MUTATION_FREQ_CHANGE)) * MAX_MUTATION_CHANCE;
	}

	inline uint calculateSessionsToEvol() const {
		return GAMES_PER_POP * m_trainee->getPopulationSize();
	}

	inline float getLayerMutationChance() const {
		return std::abs(std::sin(m_trainee->getGenerartion() * MUTATION_FREQ_CHANGE)) * MAX_LAYER_MUTATION_CHANCE;
	}

	GameResult runGame(const NNAI* white, const NNAI* black, NeuronBuffer<float>& neuronBuffer) const;
	float runGameAgainstRandom(const NNAI* ai, NeuronBuffer<float>& neuronBuffer) const;
	uint findAndStorePlayerIndex();
};
