#include "ai/ai-trainer.h"
#include "ai/ai-player.h"
#include "ai/random-player.h"
#include "tools/random-generator.h"

#include <thread>
#include <mutex>
#include <algorithm>

GameResult AITrainer::runGame(AI* white, AI* black, int cycles, int cyclesPerMove, bool useTime) {
	Board b;
	b.setupBoard();
	AIPlayer whitePlayer(Color::WHITE, white, cycles, cyclesPerMove, useTime);
	AIPlayer blackPlayer(Color::BLACK, black, cycles, cyclesPerMove, useTime);
	Game g(b, &whitePlayer, &blackPlayer);
	return g.start(false);
}

float AITrainer::runRandom(AI* ai) {
	AIPlayer whitePlayer(Color::WHITE, ai, 0, RANDOM_CYCLES, false);
	RandomPlayer random(Color::BLACK);
	Board b;
	b.setupBoard();
	Game g(b, &whitePlayer, &random);
	GameResult result = g.start(false);
	if (result == GameResult::DRAW) {
		return POINTS_FOR_RANDOM_DRAW;
	}
	if (result == GameResult::WHITE_WINS) {
		return POINTS_FOR_RANDOM_WIN;
	}
	return POINTS_FOR_RANDOM_LOSE;
}

void AITrainer::runTraining(int sessions, int threads) {
	std::mutex sessionLock;
	auto work = [&]() {
		RandomGenerator rgen;
		int ai0 = std::min((int) (m_population->getPopulationSize() * rgen.get()),
			m_population->getPopulationSize() - 1);
		int ai1 = (ai0 + 1 + std::min((int) ((m_population->getPopulationSize() - 1) * rgen.get()),
			m_population->getPopulationSize() - 2)) % m_population->getPopulationSize();
		AI* white = m_population->getAI(ai0);
		AI* black = m_population->getAI(ai1);
		float whitePoints = runRandom(white);
		float blackPoints = runRandom(black);
		for (const auto& game : GAMES) {
			GameResult result = runGame(white, black, std::get<0>(game), std::get<1>(game), std::get<2>(game));
			if (result == GameResult::DRAW) {
				continue;
			}
			float points = result == GameResult::WHITE_WINS ? std::get<3>(game) : -std::get<3>(game);
			whitePoints += points;
			blackPoints += -points;
		}
		sessionLock.lock();
		white->updateFitness(whitePoints);
		black->updateFitness(blackPoints);
		m_population->finishedTraining();
		sessionLock.unlock();
	};
}

