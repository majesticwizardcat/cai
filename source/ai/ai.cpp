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
	str += std::to_string(score) + '\n';
	return std::move(str);
}

void AISaveData::loadFromStream(std::ifstream& inputFile) {
	propagator.loadFromStream(inputFile);
	analyzer.loadFromStream(inputFile);
	fastLookup.loadFromStream(inputFile);
	cyclesManager.loadFromStream(inputFile);
	normalizer.loadFromStream(inputFile);
	inputFile >> score;
}

AI::AI() : m_propagator(PROPAGATOR_LAYOUT), m_analyzer(ANALYZER_LAYOUT),
	m_fastLookup(FAST_LOOKUP_LAYOUT), m_cyclesManager(CYCLES_MANAGER_LAYOUT),
	m_normalizer(NORMALIZER_LAYOUT), m_score(0.0f) { }

AI::AI(AI&& other) : m_propagator(std::move(other.m_propagator)),
	m_analyzer(std::move(other.m_analyzer)),
	m_fastLookup(std::move(other.m_fastLookup)),
	m_cyclesManager(std::move(other.m_cyclesManager)),
	m_normalizer(std::move(other.m_normalizer)),
	m_score(std::move(other.m_score)) { }

AI::AI(const AISaveData& saveData) : m_propagator(saveData.propagator),
	m_analyzer(saveData.analyzer), m_fastLookup(saveData.fastLookup),
	m_cyclesManager(saveData.cyclesManager), m_normalizer(saveData.normalizer),
	m_score(saveData.score) { }

AI::AI(const char* location) : AI(AISaveData(location)) { }

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
	sd.score = m_score;
	return std::move(sd);
}

int AI::cycles(int current, int max) {
	std::vector<float> input = { (float) current, (float) max };
	return (int) std::floor(m_cyclesManager.feed(input).back());
}

float AI::analyze(const std::vector<float>& position, int cycles) {
	std::vector<float> propagated = propagate(position, cycles);
	return m_analyzer.feed(propagated).back();
}

float AI::fastLookup(const std::vector<float>& position) {
	return m_fastLookup.feed(position).back();
}

std::vector<float> AI::propagate(const std::vector<float>& inputPosition, int cycles) {
	std::vector<float> pos = inputPosition;
	for (int i = 0; i < cycles; ++i) {
		pos = m_propagator.feed(pos);
	}
	std::vector<float> un(1);
	for (int i = 0; i < pos.size(); ++i) {
		un[0] = pos[i];
		pos[i] = m_normalizer.feed(un).back();
	}
	return std::move(pos);
}

bool AI::operator==(const AI& other) const {
	return util::equals(m_score, other.m_score)
		&& m_propagator == other.m_propagator
		&& m_analyzer == other.m_analyzer
		&& m_fastLookup == other.m_fastLookup
		&& m_normalizer == other.m_normalizer
		&& m_cyclesManager == other.m_cyclesManager;
}

