#pragma once

#include "game/game.h"
#include "ai/ai-trainer.h"
#include "ai/ai-player.h"
#include "ai/cai-population.h"
#include "tools/util.h"
#include "tools/testing.h"

#include <string>
#include <memory>

static const uint DEFAULT_POPULATION = 100;
static const std::string CAI_EXT = ".cai";
static const uint AI_TIME = CYCLES_PER_HOUR;

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
	void savePopulation() const;
	void loadPopulation(const std::string& name);
	void printInfo();
	void trainPopulation(int sessions);
	void trainPopulation(int sesssions, int times);
	void playGameVSAI(Color playerColor);
	void setThreads(int threads);

public:
	Cai() : m_threads(4), m_population(nullptr) { }

	void start();
};

