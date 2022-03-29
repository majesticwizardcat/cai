#include "cai.h"

#include <iostream>
#include <memory>
#include <string>
#include <vector>

void Cai::printInstructions() {
	std::cout << "Commands available:" << '\n'
		<< "\thelp: prints this message" << '\n'
		<< "\texit: terminates cai" << '\n'
		<< "\tplay: starts a player vs player game" << '\n'
		<< "\tperft [N]: starts a perft test with N depth" << '\n'
		<< "\tthreads [N]: set the number of threads to [N]" << '\n'
		<< "\tcreate [name] [size(opt)]: creates a new population" << '\n'
		<< "\tload [name]: loads ai population" << '\n'
		<< "\tsave: saves the current population" << '\n'
		<< "\tinfo: Shows current population info" << '\n'
		<< "\tplayai [color(opt)]: plays the current ai" << '\n'
		<< "\ttrain [sessions] [times(opt)]: runs [sessions] training sessions [times] times" << '\n';
}

void Cai::playGame() {
	Board b;
	b.setupBoard();
	HumanPlayer white(Color::WHITE);
	HumanPlayer black(Color::BLACK);
	Game g(b, &white, &black);
	g.start();
}

void Cai::runPerft(int depth) {
	std::string fen;
	std::cout << "Give the FEN for the position(empty for default): ";
	std::getline(std::cin, fen);
	if (fen.empty()) {
		std::cout << "Perft for depth: " << depth << " is: " << testing::perft(depth) << '\n';
	}
	else {
		std::cout << "Perft for depth: " << depth << " is: " << testing::perft(fen, depth) << '\n';
	}
}

void Cai::createPopulation(const std::string& name) {
	createPopulation(name, 0);
}

void Cai::createPopulation(const std::string& name, int population) {
	if (population <= 0) {
		population = DEFAULT_POPULATION;
	}
	else {
		m_population = createCAIPopulation(name, population);
	}
	m_populationName = name;
	savePopulation();
}

void Cai::savePopulation() const {
	if (!m_population) {
		std::cout << "No population loaded, cannot save..." << '\n';
		return;
	}
	m_population->saveToDisk();
	std::cout << "Saved population!" << '\n';
}

void Cai::loadPopulation(const std::string& name) {
	m_population = std::make_unique<CAIPopulation>(name + ".cai");
	m_populationName = name;
	std::cout << "Population loaded!" << '\n';
}

void Cai::printInfo() {
	if (!m_population) {
		std::cout << "No population loaded, cannot print info..." << '\n';
		return;
	}
	m_population->printInfo();
}

void Cai::trainPopulation(int sessions) {
	trainPopulation(sessions, 1);
}

void Cai::trainPopulation(int sessions, int times) {
	if (!m_population) {
		std::cout << "No population loaded, cannot train..." << '\n';
		return;
	}
	times = std::max(1, times);
	AITrainer trainer(sessions, m_threads, m_population.get());
	for (int i = 0; i < times; ++i) {
		trainer.run();
		savePopulation();
	}
	std::cout << "Training finished all sessions" << '\n';
}

void Cai::setThreads(int threads) {
	m_threads = threads;
}

void Cai::playGameVSAI(Color playerColor) {
	if (!m_population) {
		std::cout << "No population loaded, cannot play game..." << '\n';
		return;
	}
	Board b;
	b.setupBoard();
	Color aiColor = playerColor == Color::WHITE ? Color::BLACK : Color::WHITE;
	HumanPlayer human(playerColor);
	AIPlayer aip(aiColor, &m_population->getBestNNAiConstRef(), 0, CYCLES_PER_MINUTE);
	Player* white;
	Player* black;
	if (playerColor == Color::WHITE) {
		white = &human;
		black = &aip;
	}
	else {
		white = &aip;
		black = &human;
	}
	Game g(b, white, black);
	g.start();
}

void Cai::processCommand(const std::string& command, const std::vector<std::string>& arguments) {
	if (command == "help") {
		printInstructions();
	}
	else if (command == "exit") {
		std::cout << "Bye bye!" << '\n';
		exit(0);
	}
	else if (command == "play") {
		playGame();
	}
	else if (command == "perft") {
		if (arguments.empty() || arguments[0].empty()) {
			std::cout << "No argument for perft depth, run 'perft [depth]'" << '\n';
			return;
		}
		if (!isdigit(arguments[0][0])) {
			std::cout << "Bad argument for perft depth" << '\n';
			return;
		}
		runPerft(atoi(arguments[0].c_str()));
	}
	else if (command == "create") {
		if (arguments.empty() || arguments[0].empty()) {
			std::cout << "No arguments for population name, run 'create [name] [size(opt)]'" << '\n';
			return;
		}
		if (arguments.size() >= 2) {
			if (arguments[1].empty() || !isdigit(arguments[1][0])) {
				std::cout << "Bad arguments for population size" << '\n';
				return;
			}
			createPopulation(arguments[0], atoi(arguments[1].c_str()));
			return;
		}
		createPopulation(arguments[0]);
	}
	else if (command == "save") {
		savePopulation();
	}
	else if (command == "load") {
		if (arguments.empty() || arguments[0].empty()) {
			std::cout << "Bad arguments for load: run 'load [name]'" << '\n';
			return;
		}
		loadPopulation(arguments[0]);
	}
	else if (command == "info") {
		printInfo();
	}
	else if (command == "train") {
		if (arguments.empty() || arguments[0].empty() || !isdigit(arguments[0][0])) {
			std::cout << "Bad arguments for train: run 'train [sessions]'" << '\n';
			return;
		}
		if (arguments.size() > 1) {
			if (arguments[1].empty() || !isdigit(arguments[1][0])) {
				std::cout << "Bad arguments for train: run 'train [sessions] [times]'" << '\n';
				return;
			}
			trainPopulation(atoi(arguments[0].c_str()), atoi(arguments[1].c_str()));
			return;
		}
		trainPopulation(atoi(arguments[0].c_str()));
	}
	else if(command == "playai") {
		if (arguments.size() >= 1 && !arguments[0].empty()) {
			if (arguments[0] == "white") {
				playGameVSAI(Color::WHITE);
				return;
			}
			else if (arguments[0] == "black") {
				playGameVSAI(Color::BLACK);
				return;
			}
			std::cout << "Bad argument for color, playing with white" << '\n';
		}
		playGameVSAI(Color::WHITE);
	}
	else if (command == "threads") {
		if (arguments.empty() || arguments[0].empty() || !isdigit(arguments[0][0])) {
			std::cout << "Bad arguments for threads number, run 'threads [N]'" << '\n';
			return;
		}
		setThreads(atoi(arguments[0].c_str()));
	}
}

std::string Cai::parseCommand(std::vector<std::string>* arguments) {
	arguments->clear();
	std::string buffer;
	std::getline(std::cin, buffer);
	if (buffer.empty()) {
		return "";
	}

	std::vector<std::string> spl = util::split(buffer, " ");
	if (spl.empty()) {
		return "";
	}

	std::string command = spl[0];

	if (spl.size() > 1) {
		for (int i = 1; i < spl.size(); ++i) {
			arguments->emplace_back(spl[i]);
		}
	}
	return std::move(command);
}

void Cai::start() {
	std::string command;
	std::vector<std::string> arguments;

	while (true) {
		std::cout << "CAI>";
		command = parseCommand(&arguments);
		processCommand(command, arguments);
	}
}

int main() {
	Cai().start();
}

