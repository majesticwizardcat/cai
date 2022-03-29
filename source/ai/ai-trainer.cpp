#include "ai/ai-trainer.h"
#include "ai/ai-player.h"
#include "tools/random-generator.h"

#include <thread>
#include <mutex>
#include <algorithm>
#include <atomic>
#include <functional>

uint AITrainer::findAndStorePlayerIndex() {
	uint index;
	{
		std::lock_guard<std::mutex> lock(m_occupiedSetLock);
		index = m_indexDist(m_randomDevice);
	}
	while (true) {
		std::lock_guard<std::mutex> lock(m_occupiedSetLock);
		if (m_occupied.find(index) == m_occupied.end()) {
			m_occupied.insert(index);
			break;
		}
	}
	assert(index < m_trainee->getPopulationSize());
	return index;
}

std::vector<NNPPTrainingUpdate<float>> AITrainer::runSession() {
	std::vector<NNPPTrainingUpdate<float>> scoreUpdates;
	uint whitePlayerIndex = findAndStorePlayerIndex();
	uint blackPlayerIndex = findAndStorePlayerIndex();
	uint gameIndex;
	{
		std::lock_guard<std::mutex> lock(m_occupiedSetLock);
		gameIndex = m_gameChoiceIndex(m_randomDevice);
	}
	assert(gameIndex < GAMES.size());
	TrainTest test = GAMES[gameIndex];
	AI* white = m_trainee->getNNAiPtrAt(whitePlayerIndex);
	AI* black = m_trainee->getNNAiPtrAt(blackPlayerIndex);

	GameResult result = runGame(white, black, test);
	float pointsForWhite = 0.0f;
	float pointsForBlack = 0.0f;
	if (result == GameResult::WHITE_WINS) {
		pointsForWhite = test.points;
		pointsForBlack = -test.points;
	}
	else {
		pointsForWhite = -test.points;
		pointsForBlack = test.points;
	}

	scoreUpdates.emplace_back(white, pointsForWhite);
	scoreUpdates.emplace_back(black, pointsForBlack);

	{
		std::lock_guard<std::mutex> lock(m_occupiedSetLock);
		m_occupied.erase(whitePlayerIndex);
		m_occupied.erase(blackPlayerIndex);
	}
	return scoreUpdates;
}

uint AITrainer::sessionsTillEvolution() const {
	return calculateSessionsToEvol();
}

GameResult AITrainer::runGame(const AI* white, const AI* black, const TrainTest& test) {
	Board b;
	b.setupBoard();
	AIPlayer whitePlayer(Color::WHITE, white, test.totalCycles, test.cyclesPerMove);
	AIPlayer blackPlayer(Color::BLACK, black, test.totalCycles, test.cyclesPerMove);
	Game g(b, &whitePlayer, &blackPlayer, MAX_MOVES);
	return g.start(false);
}
