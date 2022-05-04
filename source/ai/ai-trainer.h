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
const float DRAW_NO_MOVES_POINTS_LOSS = 3.0f;
const float POINTS_PER_CYCLE = 0.1f;
const float CYCLES_PER_GEN = 0.01f;
const float SESSIONS_TO_EVOL_PER_GEN = 0.035f;
const float MAX_MUTATION_CHANCE = 0.33f;
const float MUTATION_FREQ_CHANGE = 0.025f;
const float LAYER_ADDITION_CHANCE = 0.7f;
const float LAYER_MUTATION_CHANCE = 0.05f;
const uint MAX_LAYER_MUTATION = 3;

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
	void setMutationInfo(MutationInfo* mutationInfo) const override {
		mutationInfo->weightMutationChance = getWeightMutationChance();
		mutationInfo->maxLayersMutation = MAX_LAYER_MUTATION;
		mutationInfo->layerAdditionChance = LAYER_ADDITION_CHANCE;
		mutationInfo->layerMutationChance = LAYER_MUTATION_CHANCE;
	}

private:
	std::random_device m_randomDevice;
	std::uniform_real_distribution<float> m_realDist;
	std::uniform_int_distribution<uint> m_indexDist;
	std::unordered_set<uint> m_occupied;
	std::mutex m_occupiedSetLock;

	inline float getWeightMutationChance() const {
		return std::abs(std::cos(m_trainee->getGenerartion() * MUTATION_FREQ_CHANGE)) * MAX_MUTATION_CHANCE;
	}

	inline uint calculateSessionsToEvol() const {
		return static_cast<uint>(m_trainee->getPopulationSize() * (1.0f + m_trainee->getGenerartion() * SESSIONS_TO_EVOL_PER_GEN));
	}

	inline uint cyclesToUse() {
		return std::min(CYCLES_PER_HOUR, CYCLES_PER_SECOND + static_cast<uint>(m_realDist(m_randomDevice) * m_trainee->getGenerartion() * CYCLES_PER_GEN));
	}

	GameResult runGame(const AI* white, const AI* black, uint cycles);
	uint findAndStorePlayerIndex();
};
