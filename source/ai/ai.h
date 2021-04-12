#pragma once

class AI;
struct AISaveData;

#include "ai/neural-network.h"

#include <vector>

const std::vector<int> PROPAGATOR_LAYOUT = { 64, 100, 156, 80, 64 };
const std::vector<int> ANALYZER_LAYOUT = { 64, 100, 80, 30, 10, 1 };
const std::vector<int> FAST_LOOKUP_LAYOUT = { 64, 30, 10, 1 };
const std::vector<int> CYCLES_MANAGER_LAYOUT = { 2, 4, 6, 3, 1 };

struct AISaveData {
	NNSaveData propagator;
	NNSaveData analyer;
	NNSaveData fastLookup;
	NNSaveData cyclesManage;
	float score;

	void saveToDisk(const char* location) const;
	void loadFromDisk(const char* location);
};

class AI {
private:
	NeuralNetwork m_propagator;
	NeuralNetwork m_analyzer;
	NeuralNetwork m_fastLookup;
	NeuralNetwork m_cyclesManager;
	float m_score;

public:
	AI();
	AI(const AI& other) = delete;
	AI(AI&& other);

	AISaveData toSaveData() const;
};

