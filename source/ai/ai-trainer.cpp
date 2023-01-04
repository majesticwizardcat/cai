#include "ai/ai-trainer.h"
#include "ai/ai-player.h"
#include "tools/random-generator.h"

#include <thread>
#include <mutex>
#include <algorithm>
#include <atomic>
#include <functional>

uint AITrainer::findAndStorePlayerIndex() {
	std::lock_guard<std::mutex> lock(m_occupiedSetLock);
	uint index = m_indexDist(m_randomDevice);

	while (m_occupied.find(index) != m_occupied.end()) {
		index = m_indexDist(m_randomDevice);
	}

	assert(index < m_trainee->getPopulationSize());
	m_occupied.insert(index);
	return index;
}

std::vector<NNPPTrainingUpdate<float>> AITrainer::runSession() {
	std::vector<NNPPTrainingUpdate<float>> scoreUpdates;
	uint whitePlayerIndex = findAndStorePlayerIndex();
	uint blackPlayerIndex = findAndStorePlayerIndex();

	uint cycles;
	{
		std::lock_guard<std::mutex> lock(m_occupiedSetLock);
		cycles = cyclesToUse();
	}

	AI* white = m_trainee->getNNAiPtrAt(whitePlayerIndex);
	AI* black = m_trainee->getNNAiPtrAt(blackPlayerIndex);

	GameResult result = runGame(white, black, cycles);
	const float gamePoints = cycles * POINTS_PER_CYCLE;
	float pointsForWhite = 0.0f;
	float pointsForBlack = 0.0f;
	const float whitePointsMult = calculateMultiplier(white->getScore(), black->getScore());
	const float blackPointsMult = 1.0f / whitePointsMult;

	switch (result) {
	case GameResult::WHITE_WINS:
		pointsForWhite = gamePoints + 1.0f;
		pointsForBlack = -gamePoints;
		break;
	case GameResult::BLACK_WINS:
		pointsForWhite = -gamePoints;
		pointsForBlack = gamePoints + 1.0f;
		break;
	case GameResult::DRAW:
	case GameResult::DRAW_NO_MOVES:
		pointsForWhite = white->getScore() > black->getScore() ? -whitePointsMult : whitePointsMult;
		pointsForBlack = white->getScore() > black->getScore() ? whitePointsMult : -whitePointsMult;
		break;
	default:
		break;
	case GameResult::WHITE_WINS_TIME:
	case GameResult::BLACK_WINS_TIME:
		assert(false);
		break;
	}

	scoreUpdates.emplace_back(white, pointsForWhite * whitePointsMult, false);
	scoreUpdates.emplace_back(black, pointsForBlack * blackPointsMult, false);

	{
		std::lock_guard<std::mutex> lock(m_occupiedSetLock);
		m_occupied.erase(whitePlayerIndex);
		m_occupied.erase(blackPlayerIndex);
	}
	return scoreUpdates;
}

uint AITrainer::sessionsTillEvolution() const {
	uint sessionsTillEvol = calculateSessionsToEvol();
	if (m_trainee->getSessionsTrainedThisGen() > sessionsTillEvol) {
		return 0;
	}
	return sessionsTillEvol - m_trainee->getSessionsTrainedThisGen();
}

GameResult AITrainer::runGame(const AI* white, const AI* black, uint cycles) {
	ChessBoard b;
	AIPlayer whitePlayer(Color::WHITE, white, cycles);
	AIPlayer blackPlayer(Color::BLACK, black, cycles);
	Game g(b, &whitePlayer, &blackPlayer, MAX_MOVES, false);
	return g.start(false);
}
