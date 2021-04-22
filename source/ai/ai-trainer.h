#pragma once

class AITrainer;

#include "ai/ai-population.h"
#include "game/game.h"

#include <tuple>

const int MAX_MOVES = 100;
const int CYCLES_PER_SECOND = 100;
const int CYCLES_PER_MINUTE = 60 * CYCLES_PER_SECOND;
const int CYCLES_PER_HOUR = 60 * CYCLES_PER_MINUTE;
const int RANDOM_CYCLES = 10 * CYCLES_PER_SECOND;
const float POINTS_FOR_RANDOM_WIN = 3.0f;
const float POINTS_FOR_RANDOM_DRAW = -3.0f;
const float POINTS_FOR_RANDOM_LOSE = -10.0f;

static const std::vector<std::tuple<int, int, bool, float>> GAMES = {
	std::make_tuple(0, CYCLES_PER_SECOND, false, 10.0f),
	std::make_tuple(0, 10 * CYCLES_PER_SECOND, false, 10.5f),
	std::make_tuple(0, CYCLES_PER_MINUTE, false, 15.5f),
	std::make_tuple(CYCLES_PER_MINUTE, 0, true, 7.0f),
	std::make_tuple(3 * CYCLES_PER_MINUTE, 0, true, 7.5f),
	std::make_tuple(5 * CYCLES_PER_MINUTE, 0, true, 10.0f),
	std::make_tuple(10 * CYCLES_PER_MINUTE, 0, true, 11.0f),
	std::make_tuple(15 * CYCLES_PER_MINUTE, 0, true, 11.5f),
	std::make_tuple(30 * CYCLES_PER_MINUTE, 0, true, 13.0f),
	std::make_tuple(CYCLES_PER_HOUR, 0, true, 15.0f),
	std::make_tuple(2 * CYCLES_PER_HOUR, 0, true, 17.5f),
};

class AITrainer {
private:
	AIPopulation* m_population;

	inline float sessionsPerGen(float gen) const { return std::log(gen + 1.0f) + 10.0f; }
	GameResult runGame(AI* white, AI* black, int cycles, int cyclesPerMove, bool useTime);
	float runRandom(AI*);

public:
	AITrainer() = delete;
	AITrainer(const AITrainer& other) = delete;
	AITrainer(AITrainer&& other) : m_population(std::move(other.m_population)) { }
	AITrainer(AIPopulation* population) : m_population(population) { }

	inline AIPopulation* getPopulation() { return m_population; }
	void runTraining(int sessions, int numberOfThreads);
};

