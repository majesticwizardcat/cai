#include "ai/ai-trainer.h"
#include "ai/ai-player.h"
#include "ai/random-player.h"
#include "tools/random-generator.h"

#include <thread>
#include <mutex>
#include <algorithm>
#include <atomic>
#include <functional>

GameResult AITrainer::runGame(AI* white, AI* black, int cycles, int cyclesPerMove, bool useTime) {
	Board b;
	b.setupBoard();
	AIPlayer whitePlayer(Color::WHITE, white, cycles, cyclesPerMove, useTime);
	AIPlayer blackPlayer(Color::BLACK, black, cycles, cyclesPerMove, useTime);
	Game g(b, &whitePlayer, &blackPlayer, MAX_MOVES);
	return g.start(false);
}

float AITrainer::runRandom(AI* ai) {
	AIPlayer whitePlayer(Color::WHITE, ai, 0, RANDOM_CYCLES, false);
	RandomPlayer random(Color::BLACK);
	Board b;
	b.setupBoard();
	Game g(b, &whitePlayer, &random, MAX_MOVES);
	GameResult result = g.start(false);
	if (result == GameResult::DRAW) {
		return POINTS_FOR_RANDOM_DRAW;
	}
	if (result == GameResult::WHITE_WINS) {
		return POINTS_FOR_RANDOM_WIN;
	}
	return POINTS_FOR_RANDOM_LOSE;
}

void AITrainer::runTraining(int sessions, int numberOfThreads) {
	std::mutex sessionLock;
	std::atomic<int> sessionsCompleted = 0;
	auto work = [&](int workerSessions) {
		RandomGenerator rgen;
		while (sessionsCompleted++ < workerSessions) {
			int ai0 = std::min((int) (m_population->getPopulationSize() * rgen.get()),
				m_population->getPopulationSize() - 1);
			int ai1 = (ai0 + 1 + std::min((int) ((m_population->getPopulationSize() - 1) * rgen.get()),
				m_population->getPopulationSize() - 2)) % m_population->getPopulationSize();
			AI* white = m_population->getAI(ai0);
			AI* black = m_population->getAI(ai1);
			float whitePoints = 0.0f;
			float blackPoints = 0.0f;
			int gameTypes = GAMES.size() + 1;
			int index = std::min((int) std::floor(gameTypes * rgen.get()), gameTypes - 1);
			if (index == gameTypes) {
				whitePoints += runRandom(white);
				blackPoints += runRandom(black);
			}
			else {
				const auto& game = GAMES[index];
				GameResult result = runGame(white, black, std::get<0>(game), std::get<1>(game), std::get<2>(game));
				if (result != GameResult::DRAW) {
					float points = result == GameResult::WHITE_WINS ? std::get<3>(game) : -std::get<3>(game);
					whitePoints += points;
					blackPoints += -points;
				}
			}

			sessionLock.lock();
			white->updateFitness(whitePoints);
			black->updateFitness(blackPoints);
			m_population->finishedTraining();
			sessionLock.unlock();

			std::cout << "\rSessions completed: " << sessionsCompleted << " out of: " << workerSessions;
			std::cout.flush();
		}
	};
	int generationSessions = (int) (sessionsPerGen((float) m_population->getGeneration()) * (float) m_population->getPopulationSize());
	int sessionsTillNextGen = generationSessions - m_population->getTrainingSessions();
	int runSessions = sessions == 0 ? sessionsTillNextGen : std::min(sessions, sessionsTillNextGen);
	std::cout << "Starting training sessions: " << runSessions << " with: " << numberOfThreads << " threads" << '\n';
	std::vector<std::thread> threads;
	std::cout << "\rSessions completed: 0 out of " << runSessions;
	std::cout.flush();
	for (int i = 0; i < numberOfThreads; ++i) {
		threads.push_back(std::thread(work, runSessions));
	}
	for (auto& thread : threads) {
		thread.join();
	}
	std::cout << "\rSessions completed: " << runSessions << " out of: " << runSessions << '\n';
	if (m_population->getTrainingSessions() >= generationSessions) {
		std::cout << "Repopulating..." << '\n';
		m_population->repopulate();
		std::cout << "Repopulation completed" << '\n';
	}
	std::cout << "Finished training, best fitness: " << m_population->getBestAI()->getFitness() << '\n';
}

