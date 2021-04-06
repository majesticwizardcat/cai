#pragma once

#include "game/board.h"

#include <string>
#include <iostream>
#include <functional>

const std::string DEFAULT_POSITION = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";

namespace testing {

unsigned long long perft(const std::string& fen, int depth) {
	std::function<unsigned long long(const Board&, int, Color)>
		findPositions = [&](const Board& b, int depth, Color nextPlayer) {
		auto moves = b.getMoves(nextPlayer);
		if (depth == 0) {
			return moves.size();
		}

		unsigned long long positions = 0;
		for (const auto& m : moves) {
			Board nextPosition(b);
			nextPosition.playMove(m);
			positions += findPositions(nextPosition, depth - 1,
				nextPlayer == Color::WHITE ? Color::BLACK : Color::WHITE);
		}
		return positions;
	};
	Board start;
	Color next = start.setPosition(fen);
	return findPositions(start, depth, next);
}

unsigned long long perft(int depth) {
	return perft(DEFAULT_POSITION, depth);
}

};
