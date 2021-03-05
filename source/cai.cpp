#include "game/game.h"
#include "tools/util.h"

#include <iostream>
#include <memory>
#include <string>
#include <vector>

void printInstructions() {
	std::cout << "Commands available:" << '\n'
		<< "\thelp: prints this message" << '\n'
		<< "\texit: terminates cai" << '\n'
		<< "\tplay: starts a player vs player game" << '\n'
		<< "\tperft N: starts a perft test with N depth" << '\n';
}

void playGame() {
	Board b;
	b.setupBoard();
	HumanPlayer white(Color::WHITE);
	HumanPlayer black(Color::BLACK);
	Game g(b, &white, &black);
	g.start();
}

void processCommand(const std::string& command, const std::vector<std::string>& arguments) {
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
		std::cout << "perft is currently unavailable" << '\n';
	}
}

std::string parseCommand(std::vector<std::string>* arguments) {
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

int main() {
	std::string command;
	std::vector<std::string> arguments;

	while (true) {
		std::cout << "CAI>";
		command = parseCommand(&arguments);
		processCommand(command, arguments);
	}
}

