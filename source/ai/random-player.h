#pragma once

class RandomPlayer;

#include "game/player.h"
#include "tools/random-generator.h"

#include <algorithm>

class RandomPlayer : public Player {
private:
	RandomGenerator m_rgen;

public:
	RandomPlayer() = delete;
	RandomPlayer(Color color) : Player(color) { }

	bool getMove(const Board& board, Move* move) {
		auto moves = board.getMoves(m_color);
		if (moves.empty()) {
			return false;
		}
		int numMoves = moves.size();
		int moveIndex = std::min((int) (m_rgen.get() * moves.size()), numMoves - 1);
		*move = moves[moveIndex];
		return true;
	}
};

