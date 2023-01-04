#pragma once

class AITrainer;

#include "ai/cai-population.h"
#include "ai/ai-player.h"
#include "game/game.h"

#include <nnpp.hpp>

#include <tuple>
#include <unordered_set>
#include <mutex>
#include <random>
#include <algorithm>

const uint MAX_MOVES = 256;
const uint CYCLES_PER_SECOND = 35;
const uint CYCLES_PER_MINUTE = 60 * CYCLES_PER_SECOND;
const uint CYCLES_PER_HOUR = 60 * CYCLES_PER_MINUTE;
const float POINTS_PER_CYCLE = 0.09f;
const float CYCLES_PER_GEN = 0.01f;
const float MAX_MUTATION_CHANCE = 0.2f;
const float MUTATION_FREQ_CHANGE = 0.1f;
const float LAYER_ADDITION_CHANCE = 1.0f;
const float MAX_LAYER_MUTATION_CHANCE = 0.1f;
const uint MAX_LAYER_MUTATION = 2;
const uint GAMES_PER_POP = 10;
const uint TRAINING_SESSIONS_REQUIRED = GAMES_PER_POP;
const float CHILD_REGRESSION = 0.9f;

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

class AITrainer : public NNPPTrainer<float> {
public:
	AITrainer() = delete;
	AITrainer(const AITrainer& other) = delete;
	AITrainer(uint sessions, uint threads, CAIPopulation* const population) 
		: NNPPTrainer<float>(sessions, threads, population)
		, m_indexDist(0, m_trainee->getPopulationSize() - 1)
		, m_realDist(0.0f, 1.0f) { }

protected:
	std::vector<NNPPTrainingUpdate<float>> runSession() override;
	uint sessionsTillEvolution() const override;
	float getAvgScoreImportance() const override { return 0.5f; }

	void setEvolutionInfo(EvolutionInfo* evolutionInfo) const override {
		NNPPTrainer<float>::setEvolutionInfo(evolutionInfo);
		evolutionInfo->weightMutationChance = getWeightMutationChance();
		evolutionInfo->maxLayersMutation = MAX_LAYER_MUTATION;
		evolutionInfo->layerAdditionChance = LAYER_ADDITION_CHANCE;
		evolutionInfo->layerMutationChance = getLayerMutationChance();
		evolutionInfo->childRegressionPercentage = CHILD_REGRESSION;
		evolutionInfo->minTrainingSessionsRequired = TRAINING_SESSIONS_REQUIRED;
	}

private:
	std::random_device m_randomDevice;
	std::uniform_real_distribution<float> m_realDist;
	std::uniform_int_distribution<uint> m_indexDist;
	std::unordered_set<uint> m_occupied;
	std::mutex m_occupiedSetLock;

	inline static float calculateMultiplier(float score0, float score1) {
		const float alpha = 0.007f;
		float absDif = std::abs(score0 - score1);
		float mul = absDif >= 300.0f ? 0.1f : std::sqrt(1.0f / std::exp(absDif * alpha));
		assert(mul >= 0.1f);
		return score0 > score1 ? mul : 1.0f / mul;
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

	inline uint cyclesToUse() {
		return std::min(CYCLES_PER_HOUR, CYCLES_PER_SECOND + static_cast<uint>(m_realDist(m_randomDevice) * m_trainee->getGenerartion() * CYCLES_PER_GEN));
	}

	GameResult runGame(const AI* white, const AI* black, uint cycles);
	uint findAndStorePlayerIndex();
};
