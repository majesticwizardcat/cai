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

	switch (result) {
	case GameResult::WHITE_WINS:
		pointsForWhite = test.points;
		pointsForBlack = -test.points;
		break;
	case GameResult::WHITE_WINS_TIME:
		pointsForWhite = test.points * TIME_WIN_MOD;
		pointsForBlack = -test.points * TIME_WIN_MOD;
		break;
	case GameResult::BLACK_WINS:
		pointsForWhite = -test.points;
		pointsForBlack = test.points;
		break;
	case GameResult::BLACK_WINS_TIME:
		pointsForWhite = -test.points * TIME_WIN_MOD;
		pointsForBlack = test.points * TIME_WIN_MOD;
		break;
	case GameResult::DRAW_NO_MOVES:
		pointsForWhite = -DRAW_NO_MOVES_POINTS_LOSS;
		pointsForBlack = -DRAW_NO_MOVES_POINTS_LOSS;
	default:
		break;
	}

	scoreUpdates.emplace_back(white, pointsForWhite, false);
	scoreUpdates.emplace_back(black, pointsForBlack, false);

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

GameResult AITrainer::runGame(const AI* white, const AI* black, const TrainTest& test) {
	ChessBoard b;
	b.setupBoard();
	AIPlayer whitePlayer(Color::WHITE, white, test.totalCycles, test.cyclesPerMove);
	AIPlayer blackPlayer(Color::BLACK, black, test.totalCycles, test.cyclesPerMove);
	Game g(b, &whitePlayer, &blackPlayer, MAX_MOVES);
	return g.start(false, false);
}
