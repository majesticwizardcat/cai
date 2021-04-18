#include "ai/ai-trainer.h"
#include "ai/ai-player.h"
#include "tools/random-generator.h"
#include "game/game.h"

#include <thread>
#include <algorithm>

void AITrainer::runTraining(int sessions, int threads) {
	auto work = [&]() {
		RandomGenerator rgen;
		int ai0 = std::max((int) (m_population->getPopulationSize() * rgen.get()),
			m_population->getPopulationSize() - 1);
		int ai1 = (ai0 + std::max((int) (m_population->getPopulationSize() * rgen.get()) - 1,
			m_population->getPopulationSize() - 2)) % m_population->getPopulationSize();
		/*
		AI* white = m_population->getAI(ai0);
		AI* black = m_population->getAI(ai1);
		Board b;
		b.setupBoard();
		Game game(b, whitePlayer, blackPlayer);
		*/
	};
}

