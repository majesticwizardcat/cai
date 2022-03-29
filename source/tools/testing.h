#pragma once

#include "game/board.h"

#include <string>
#include <iostream>
#include <functional>

const std::string DEFAULT_POSITION = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";

namespace testing {

unsigned long long perft(const std::string& fen, int startingDepth) {
	std::function<unsigned long long(const Board&, int, Color)>
		findPositions = [&](const Board& b, int depth, Color nextPlayer) {
		auto moves = b.getMoves(nextPlayer);
		if (depth == 1) {
			return static_cast<unsigned long long>(moves.size());
		}
		unsigned long long positions = 0;
		int curMoveIndex = 0;
		for (const auto& m : moves) {
			Board nextPosition(b);
			nextPosition.playMove(m);
			positions += findPositions(nextPosition, depth - 1, nextPlayer == Color::WHITE ? Color::BLACK : Color::WHITE);
		}
		return positions;
	};
	Board start;
	Color next = start.setPosition(fen);
	start.printBoard();
	std::cout << "Starting perft test with depth " << startingDepth << '\n';
	unsigned long long positionsFound = findPositions(start, startingDepth, next);
	std::cout << "Positions: " << positionsFound << '\n';
	return positionsFound;
}

unsigned long long perft(int depth) {
	return perft(DEFAULT_POSITION, depth);
}

};
