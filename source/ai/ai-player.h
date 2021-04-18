#pragma once

class AIPlayer;

#include "game/player.h"
#include "ai/ai.h"
#include "tools/random-generator.h"

const int DEFAULT_CYCLES = 6000;

class AIPlayer : public Player {
private:
	AI* m_ai;
	int m_cycles;
	int m_maxCycles;
	int m_cyclesPerMove;
	bool m_useCycles;
	RandomGenerator m_rgen;

public:
	AIPlayer() = delete;
	AIPlayer(Color color, AI* ai) : AIPlayer(color, ai, DEFAULT_CYCLES, 0, false) { }
	AIPlayer(Color color, AI* ai, int cycles) : AIPlayer(color, ai, cycles, 0, false) { }
	AIPlayer(Color color, AI* ai, int cycles, int cyclesPerMove)
		: AIPlayer(color, ai, cycles, cyclesPerMove, true) { }
	AIPlayer(Color color, AI* ai, int cycles, int cyclesPerMove, bool useCycles) : Player(color),
		m_ai(ai), m_cycles(cycles), m_cyclesPerMove(cyclesPerMove), m_useCycles(useCycles),
		m_maxCycles(cycles) { }

	bool getMove(const Board& board, Move* move);
	inline int getCycles() const { return m_cycles; }
};

