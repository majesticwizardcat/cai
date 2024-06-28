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

class NNAITrainer : public nnpp::NNPPTrainer<float> {
public:
	NNAITrainer() = delete;
	NNAITrainer(const AITrainer& other) = delete;
	NNAITrainer(uint32_t sessions, uint32_t threads, CAIPopulation* const population) 
			: NNPPTrainer<float>(sessions, threads, *population, nnpp::getDefaultEvolutionInfoFloat())
			, m_indexDist(0, m_trainee.getPopulationSize() - 1)
			, m_realDist(0.0f, 1.0f) { }

protected:
	std::vector<nnpp::NNPPTrainingUpdate<float>> runSession(nnpp::NeuronBuffer<float>& neuronBuffer) override final;
	uint64_t sessionsTillEvolution() const override final;
	float getAvgScoreImportance() const override final { return 0.5f; }

	void setEvolutionInfo(nnpp::EvolutionInfo<float>& evolutionInfo) const override final {
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
	static constexpr float POINTS_PER_GAME = 25.0f;
	static constexpr float DRAW_POINTS = 3.5f;
	static constexpr float RANDOM_WIN_POINTS = 7.0f;
	static constexpr float MAX_MUTATION_CHANCE = 0.2f;
	static constexpr float MUTATION_FREQ_CHANGE = 0.1f;
	static constexpr float LAYER_ADDITION_CHANCE = 1.0f;
	static constexpr float MAX_LAYER_MUTATION_CHANCE = 0.1f;
	static constexpr float MIN_MUTATION_VALUE = MIN_AI_WEIGHT_VALUE * 15.0f;
	static constexpr float MAX_MUTATION_VALUE = MAX_AI_WEIGHT_VALUE * 15.0f;
	static constexpr uint32_t MAX_LAYER_MUTATION = 2;
	static constexpr uint32_t GAMES_PER_POP = 20;
	static constexpr uint32_t TRAINING_SESSIONS_REQUIRED = GAMES_PER_POP;
	static constexpr float CHILD_REGRESSION = 0.8f;
	static constexpr uint32_t MAX_MOVES_PER_GAME = 160;
	static constexpr uint32_t MAX_MOVES_PER_RANDOM_GAME = 80;

	pcg32_fast m_randomDevice;
	std::uniform_real_distribution<float> m_realDist;
	std::uniform_int_distribution<uint> m_indexDist;
	std::unordered_set<uint> m_occupied;
	std::mutex m_occupiedSetLock;

	inline static constexpr float calculatePoints(const float score0, const float score1) {
		constexpr float alpha = 0.007f;
		const float absDif = std::abs(score0 - score1);
		const float mul = absDif >= 300.0f ? 0.1f : std::sqrt(1.0f / std::exp(absDif * alpha));
		assert(mul >= 0.1f);
		return mul * POINTS_PER_GAME;
	}

	constexpr float getWeightMutationChance() const {
		return std::abs(std::cos(m_trainee.getGenerartion() * MUTATION_FREQ_CHANGE)) * MAX_MUTATION_CHANCE;
	}

	constexpr uint32_t calculateSessionsToEvol() const {
		return GAMES_PER_POP * m_trainee.getPopulationSize();
	}

	constexpr float getLayerMutationChance() const {
		return std::abs(std::sin(m_trainee.getGenerartion() * MUTATION_FREQ_CHANGE)) * MAX_LAYER_MUTATION_CHANCE;
	}

	GameResult runGame(const NNAI* white, const NNAI* black, nnpp::NeuronBuffer<float>& neuronBuffer) const;
	float runGameAgainstRandom(const NNAI* ai, nnpp::NeuronBuffer<float>& neuronBuffer) const;
	uint32_t findAndStorePlayerIndex();
};
