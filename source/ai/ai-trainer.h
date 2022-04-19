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

const uint MAX_MOVES = 256;
const uint CYCLES_PER_SECOND = 100;
const uint CYCLES_PER_MINUTE = 60 * CYCLES_PER_SECOND;
const uint CYCLES_PER_HOUR = 60 * CYCLES_PER_MINUTE;
const uint RANDOM_CYCLES = 10 * CYCLES_PER_SECOND;
const float TIME_WIN_MOD = 0.1f;
const float DRAW_NO_MOVES_POINTS_LOSS = 3.0f;

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
	TrainTest(0, 2 * CYCLES_PER_SECOND, 10.0f),
	TrainTest(0, 10 * CYCLES_PER_SECOND, 11.5f),
	TrainTest(0, CYCLES_PER_MINUTE, 17.5f),
	TrainTest(CYCLES_PER_MINUTE, 0, 3.5f),
	TrainTest(3 * CYCLES_PER_MINUTE, 0, 5.0f),
	TrainTest(5 * CYCLES_PER_MINUTE, 0, 10.0f),
	TrainTest(10 * CYCLES_PER_MINUTE, 0, 15.0f),
	TrainTest(15 * CYCLES_PER_MINUTE, 0, 17.5f),
	TrainTest(30 * CYCLES_PER_MINUTE, 0, 20.0f),
	TrainTest(CYCLES_PER_HOUR, 0, 21.5f),
	TrainTest(2 * CYCLES_PER_HOUR, 0, 23.5f),
	TrainTest(4 * CYCLES_PER_HOUR, 0, 25.0f),
	TrainTest(10 * CYCLES_PER_HOUR, 0, 50.0f),
};

class AITrainer : public NNPPTrainer<float> {
public:
	AITrainer() = delete;
	AITrainer(const AITrainer& other) = delete;
	AITrainer(uint sessions, uint threads, CAIPopulation* const population) 
		: NNPPTrainer<float>(sessions, threads, population),
		m_indexDist(0, m_trainee->getPopulationSize() - 1),
		m_gameChoiceIndex(0, GAMES.size() - 1) { }

protected:
	std::vector<NNPPTrainingUpdate<float>> runSession();
	uint sessionsTillEvolution() const;

private:
	std::random_device m_randomDevice;
	std::uniform_int_distribution<uint> m_indexDist;
	std::unordered_set<uint> m_occupied;
	std::mutex m_occupiedSetLock;
	std::uniform_int_distribution<uint> m_gameChoiceIndex;

	inline uint calculateSessionsToEvol() const { return static_cast<uint>(m_trainee->getPopulationSize() * (1.0f + m_trainee->getGenerartion() * 0.1f)); }

	GameResult runGame(const AI* white, const AI* black, const TrainTest& test);
	uint findAndStorePlayerIndex();
};
