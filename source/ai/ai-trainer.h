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

const int MAX_MOVES = 256;
const int CYCLES_PER_SECOND = 100;
const int CYCLES_PER_MINUTE = 60 * CYCLES_PER_SECOND;
const int CYCLES_PER_HOUR = 60 * CYCLES_PER_MINUTE;
const int RANDOM_CYCLES = 10 * CYCLES_PER_SECOND;
const float POINTS_FOR_RANDOM_WIN = 3.0f;
const float POINTS_FOR_RANDOM_DRAW = -3.0f;
const float POINTS_FOR_RANDOM_LOSE = -10.0f;

struct TrainTest {
public:
	uint totalCycles;
	uint cyclesPerMove;
	float points;

	TrainTest() {}
	TrainTest(uint totalCycles, uint cyclesPerMove, float points)
		: totalCycles(totalCycles), cyclesPerMove(cyclesPerMove), points(points) { }
};

static const std::vector<TrainTest> GAMES = {
	TrainTest(0, CYCLES_PER_SECOND, 10.0f),
	TrainTest(0, 10 * CYCLES_PER_SECOND, 10.5f),
	TrainTest(0, CYCLES_PER_MINUTE, 15.5f),
	TrainTest(CYCLES_PER_MINUTE, 0, 7.0f),
	TrainTest(3 * CYCLES_PER_MINUTE, 0, 7.5f),
	TrainTest(5 * CYCLES_PER_MINUTE, 0, 10.0f),
	TrainTest(10 * CYCLES_PER_MINUTE, 0, 11.0f),
	TrainTest(15 * CYCLES_PER_MINUTE, 0, 11.5f),
	TrainTest(30 * CYCLES_PER_MINUTE, 0, 13.0f),
	TrainTest(CYCLES_PER_HOUR, 0, 15.0f),
	TrainTest(2 * CYCLES_PER_HOUR, 0, 17.5f),
};

class AITrainer : public NNPPTrainer<float> {
public:
	AITrainer() = delete;
	AITrainer(const AITrainer& other) = delete;
	AITrainer(uint sessions, uint threads, CAIPopulation* const population) 
		: NNPPTrainer<float>(sessions, threads, population),
		m_indexDist(0, m_trainee->getPopulationSize()),
		m_gameChoiceIndex(0, GAMES.size()) { }

protected:
	std::vector<NNPPTrainingUpdate<float>> runSession();
	uint sessionsTillEvolution() const;

private:
	std::random_device m_randomDevice;
	std::uniform_int_distribution<uint> m_indexDist;
	std::unordered_set<uint> m_occupied;
	std::mutex m_occupiedSetLock;
	std::uniform_int_distribution<uint> m_gameChoiceIndex;

	inline uint calculateSessionsToEvol() const { return static_cast<uint>(m_trainee->getPopulationSize() * (1.0f * m_trainee->getGenerartion() * 0.1f)); }

	GameResult runGame(const AI* white, const AI* black, const TrainTest& test);
	uint findAndStorePlayerIndex();
};
