#include "ai/ai.h"
#include "tools/util.h"

#include <utility>

void AISaveData::saveToDisk(const char* location) const {
	std::ofstream saveFile(location);
	saveFile << asString();
	saveFile.close();
}

void AISaveData::loadFromDisk(const char* location) {
	std::ifstream inFile(location);
	if (!inFile) {
		std::cout << "Could not open file to load, returning..." << '\n';
		return;
	}
	loadFromStream(inFile);
	inFile.close();
}

std::string AISaveData::asString() const {
	std::string str;
	str += propagator.asString() + '\n';
	str += analyzer.asString() + '\n';
	str += fastLookup.asString() + '\n';
	str += cyclesManager.asString() + '\n';
	str += normalizer.asString() + '\n';
	str += std::to_string(fitness) + '\n';
	return std::move(str);
}

void AISaveData::loadFromStream(std::ifstream& inputFile) {
	propagator.loadFromStream(inputFile);
	analyzer.loadFromStream(inputFile);
	fastLookup.loadFromStream(inputFile);
	cyclesManager.loadFromStream(inputFile);
	normalizer.loadFromStream(inputFile);
	inputFile >> fitness;
}

AI::AI() : m_propagator(PROPAGATOR_LAYOUT), m_analyzer(ANALYZER_LAYOUT),
	m_fastLookup(FAST_LOOKUP_LAYOUT), m_cyclesManager(CYCLES_MANAGER_LAYOUT),
	m_normalizer(NORMALIZER_LAYOUT), m_fitness(0.0f) { }

AI::AI(AI&& other) noexcept : m_propagator(std::move(other.m_propagator)),
	m_analyzer(std::move(other.m_analyzer)),
	m_fastLookup(std::move(other.m_fastLookup)),
	m_cyclesManager(std::move(other.m_cyclesManager)),
	m_normalizer(std::move(other.m_normalizer)),
	m_fitness(std::move(other.m_fitness)) { }

AI::AI(const AISaveData& saveData) : m_propagator(saveData.propagator),
	m_analyzer(saveData.analyzer), m_fastLookup(saveData.fastLookup),
	m_cyclesManager(saveData.cyclesManager), m_normalizer(saveData.normalizer),
	m_fitness(saveData.fitness) { }

AI::AI(const char* location) : AI(AISaveData(location)) { }

AI::AI(const AI& ai0, const AI& ai1, float mutationProb)
	: m_propagator(ai0.m_propagator, ai1.m_propagator, mutationProb),
	m_analyzer(ai0.m_analyzer, ai1.m_analyzer, mutationProb),
	m_fastLookup(ai0.m_fastLookup, ai1.m_fastLookup, mutationProb),
	m_cyclesManager(ai0.m_cyclesManager, ai1.m_cyclesManager, mutationProb),
	m_normalizer(ai0.m_normalizer, ai1.m_normalizer, mutationProb),
	m_fitness((ai0.m_fitness + ai1.m_fitness) / 2.0f) { }

void AI::save(const char* location) const {
	toSaveData().saveToDisk(location);
}

AISaveData AI::toSaveData() const {
	AISaveData sd;
	sd.propagator = m_propagator.toSaveData();
	sd.analyzer = m_analyzer.toSaveData();
	sd.fastLookup = m_fastLookup.toSaveData();
	sd.cyclesManager = m_cyclesManager.toSaveData();
	sd.normalizer = m_normalizer.toSaveData();
	sd.fitness = m_fitness;
	return std::move(sd);
}

float AI::cycles(float current, float move) {
	std::vector<float> input = { (float) current, (float) move };
	return (int) std::abs(std::floor(m_cyclesManager.feed(input).back()));
}

float AI::analyze(const std::vector<float>& position, int cycles) {
	std::vector<float> propagated = propagate(position, cycles);
	return m_analyzer.feed(propagated).back();
}

float AI::fastLookup(const std::vector<float>& position) {
	return m_fastLookup.feed(position).back();
}

std::vector<float> AI::propagate(const std::vector<float>& inputPosition, int cycles) {
	std::vector<float> pos(inputPosition);
	std::vector<float> un = { 0.0f };
	for (int i = 0; i < cycles; ++i) {
		pos = m_propagator.feed(pos);
	}
	return std::move(pos);
}

bool AI::operator==(const AI& other) const {
	if (!util::equals(m_fitness, other.m_fitness)) {
		std::cout << "Different fitness values: " << m_fitness << " != " << other.m_fitness << '\n';
		return false;
	}
	return	m_propagator == other.m_propagator
		&& m_analyzer == other.m_analyzer
		&& m_fastLookup == other.m_fastLookup
		&& m_normalizer == other.m_normalizer
		&& m_cyclesManager == other.m_cyclesManager;
}

