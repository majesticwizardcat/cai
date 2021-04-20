#pragma once

#include "game/game.h"
#include "ai/ai-trainer.h"
#include "tools/util.h"
#include "tools/testing.h"

#include <string>
#include <memory>

class Cai {
private:
	std::unique_ptr<AIPopulation> m_population;
	std::string m_populationName;

	void printInstructions();
	void playGame();
	void runPerft(int depth);
	std::string parseCommand(std::vector<std::string>* arguments);
	void processCommand(const std::string& command, const std::vector<std::string>& arguments);
	void createPopulation(const std::string& name, int population);
	void createPopulation(const std::string& name);
	void savePopulation();
	void loadPopulation(const std::string& name);
	void printInfo();
	void trainPopulation(int sessions);
	void trainPopulation(int sesssions, int times);

public:
	Cai() : m_population(nullptr) { }

	void start();
};

