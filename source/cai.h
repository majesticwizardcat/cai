#pragma once

#include "game/game.h"
#include "neural-net-ai/nnai-trainer.h"
#include "neural-net-ai/nnai-player.h"
#include "neural-net-ai/cai-population.h"
#include "tools/util.h"
#include "tools/testing.h"

#include <string>
#include <memory>

static const uint64_t VERSION = 10;
static const uint DEFAULT_POPULATION = 100;
static const std::string CAI_EXT = ".cai";

class Cai {
private:
	std::unique_ptr<CAIPopulation> m_population;
	int m_threads;

	void printInstructions();
	void playGame();
	void runPerft(int depth);
	std::string parseCommand(std::vector<std::string>* arguments);
	void processCommand(const std::string& command, const std::vector<std::string>& arguments);
	void createPopulation(const std::string& name, int population);
	void createPopulation(const std::string& name);
	void savePopulation(bool verbose) const;
	void loadPopulation(const std::string& name);
	void printInfo();
	void trainPopulation(int sessions);
	void trainPopulation(int sesssions, int times);
	void playGameVSAI(Color playerColor);
	void setThreads(int threads);
	void printLayers() const;

public:
	Cai() : m_threads(4), m_population(nullptr) { }

	void start();
};

