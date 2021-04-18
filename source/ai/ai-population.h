#pragma once

class AIPopulation;
struct AIPopulationSaveData;

#include "ai/ai.h"

#include <vector>
#include <fstream>
#include <iostream>

const int DEFAULT_POPULATION_SIZE = 333;
const float MIN_MUTATION_PROB = 0.0123f;
const float MAX_MUTATION_PROB = 0.1234f;

struct AIPopulationSaveData {
	std::vector<AISaveData> population;
	int generation;
	int trainingSessions;

	AIPopulationSaveData() { }
	AIPopulationSaveData(const char* location) {
		loadFromDisk(location);
	}

	void loadFromDisk(const char* location);
	void writeToDisk(const char* location);
	void loadFromStream(std::ifstream& in);
	void writeToStream(std::ofstream& out);
};

class AIPopulation {
private:
	std::vector<AI> m_population;
	int m_generation;
	int m_trainingSessions;

	inline float mutationProb() const {
		float x = 0.1f * (float) (m_generation);
		float per = std::abs(std::cos(x)) / (x * 0.1f + 1.0f);
		return MIN_MUTATION_PROB + MAX_MUTATION_PROB * per;
	}
	AI* findBest();

public:
	AIPopulation() : AIPopulation(DEFAULT_POPULATION_SIZE) { }
	AIPopulation(const AIPopulation& other) = delete;
	AIPopulation(AIPopulation&& other) noexcept : m_population(std::move(other.m_population)),
		m_generation(std::move(other.m_generation)), 
		m_trainingSessions(std::move(other.m_trainingSessions)) { }
	AIPopulation(int size);
	AIPopulation(const char* location);
	AIPopulation(const AIPopulationSaveData& saveData);
	
	AIPopulationSaveData toSaveData() const;
	void save(const char* location) const;
	bool operator==(const AIPopulation& other) const;
	void buildNew();
	void loadFromDisk(const char* location);
	void loadFromSaveData(const AIPopulationSaveData& saveData);
	void repopulate();
	inline AI* getBestAI() { return findBest(); }
	inline float getBestFitness() { return findBest()->getFitness(); }
	inline int getGeneration() const { return m_generation; }
	inline AI* getAI(int index) { return &m_population[index]; }
	inline int getPopulationSize() const { return m_population.size(); }
	inline int getTrainingSessions() const { return m_trainingSessions; }
	inline int finishedTraining() const { return m_trainingSessions; }
	inline void printInfo() { std::cout << "Population size: " << m_population.size() << ", Generation : "
		<< m_generation << ", Training sessions for current generation: " << m_trainingSessions << " Best AI fitness: "
		<< getBestFitness() << '\n';
	}
};

