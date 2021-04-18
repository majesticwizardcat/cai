#pragma once

class AITrainer;

#include "ai/ai-population.h"

class AITrainer {
private:
	AIPopulation* m_population;

public:
	AITrainer() = delete;
	AITrainer(const AITrainer& other) = delete;
	AITrainer(AITrainer&& other) : m_population(std::move(other.m_population)) { }
	AITrainer(AIPopulation* population) : m_population(population) { }

	inline AIPopulation* getPopulation() { return m_population; }
	void runTraining(int sessions, int threads);
};

