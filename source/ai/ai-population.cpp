#include "ai/ai-population.h"
#include "tools/random-generator.h"

#include <algorithm>

void AIPopulationSaveData::loadFromDisk(const char* location) {
	std::ifstream inputFile(location);
	if (!inputFile) {
		std::cout << "Could not open file to load, returning..." << '\n';
		return;
	}
	loadFromStream(inputFile);
	inputFile.close();
}

void AIPopulationSaveData::writeToDisk(const char* location) {
	std::ofstream outFile(location);
	writeToStream(outFile);
	outFile.close();
}

void AIPopulationSaveData::loadFromStream(std::ifstream& in) {
	int populationSize;
	in >> generation;
	in >> trainingSessions;
	in >> populationSize;
	for (int i = 0; i < populationSize; ++i) {
		population.emplace_back(in);
	}
}

void AIPopulationSaveData::writeToStream(std::ofstream& out) {
	out << generation << '\n';
	out << trainingSessions << '\n';
	out << population.size() << '\n';
	for (const auto& ai : population) {
		out << ai.asString();
	}
}

AIPopulation::AIPopulation(int size) : m_generation(0), m_trainingSessions(0), m_population(size) { }
AIPopulation::AIPopulation(const char* location) : AIPopulation(AIPopulationSaveData(location)) { }
AIPopulation::AIPopulation(const AIPopulationSaveData& saveData) : m_generation(saveData.generation),
	m_trainingSessions(saveData.trainingSessions) {
	for (const auto& sd : saveData.population) {
		m_population.emplace_back(sd);
	}
}

AIPopulationSaveData AIPopulation::toSaveData() const {
	AIPopulationSaveData sd;
	sd.population.reserve(m_population.size());
	sd.generation = m_generation;
	for (const auto& ai : m_population) {
		sd.population.push_back(ai.toSaveData());
	}
	return std::move(sd);
}

void AIPopulation::save(const char* location) const {
	toSaveData().writeToDisk(location);
}

bool AIPopulation::operator==(const AIPopulation& other) const {
	if (m_generation != other.m_generation) {
		std::cout << "Different generations: " << m_generation << " != " << other.m_generation << '\n';
		return false;
	}
	if (m_population.size() != other.m_population.size()) {
		std::cout << "Different population size: "
			<< m_population.size() << " != " << other.m_population.size() << '\n';
		return false;
	}
	for (int i = 0; i < m_population.size(); ++i) {
		if (!(m_population[i] == other.m_population[i])) {
			return false;
		}
	}
	return true;
}

AI* AIPopulation::findBest() {
	auto cmp = [](const AI& ai0, const AI& ai1) {
		return ai0.getFitness() > ai1.getFitness();
	};
	auto best = std::max_element(m_population.begin(), m_population.end(), cmp);
	return &(*best);
}

void AIPopulation::buildNew() {
	m_generation = 0;
	m_trainingSessions = 0;
	m_population.clear();
	m_population = std::vector<AI>(DEFAULT_POPULATION_SIZE);
}

void AIPopulation::loadFromDisk(const char* location) {
	loadFromSaveData(AIPopulationSaveData(location));
}

void AIPopulation::loadFromSaveData(const AIPopulationSaveData& saveData) {
	m_population.clear();
	m_generation = saveData.generation;
	m_trainingSessions = saveData.trainingSessions;
	for (const auto& sd : saveData.population) {
		m_population.emplace_back(sd);
	}
}

void AIPopulation::repopulate() {
	RandomGenerator rgen;
	auto cmp = [](const AI& ai0, const AI& ai1) {
		return ai0.getFitness() > ai1.getFitness();
	};
	int initPopulation = m_population.size();
	std::sort(m_population.begin(), m_population.end(), cmp);
	m_population.erase(m_population.begin() + m_population.size() * 0.66f,
		m_population.end());
	float highestFitness = m_population.begin()->getFitness();
	int currentPopulation = m_population.size();
	auto findIndex = [&]() {
		int index;
		do {
			index = std::min((int) (rgen.get() * currentPopulation), currentPopulation);
		} while(rgen.get() > m_population[index].getFitness() / highestFitness);
		return index;
	};
	int extras = initPopulation - m_population.size() + 1;
	float pom = mutationProb();
	for (int i = 0; i < extras; ++i) {
		int ai0 = findIndex();
		int ai1 = findIndex();
		m_population.emplace_back(m_population[ai0], m_population[ai1], pom);
	}
	m_generation++;
	m_trainingSessions = 0;
}

