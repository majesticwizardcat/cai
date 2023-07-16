#include "neural-net-ai/nnai-trainer.h"
#include "neural-net-ai/nnai-player.h"
#include "tools/random-generator.h"

#include <thread>
#include <mutex>
#include <algorithm>
#include <atomic>
#include <functional>

uint NNAITrainer::findAndStorePlayerIndex() {
	std::lock_guard<std::mutex> lock(m_occupiedSetLock);
	uint index = m_indexDist(m_randomDevice);

	while (m_occupied.find(index) != m_occupied.end()) {
		index = m_indexDist(m_randomDevice);
	}

	assert(index < m_trainee->getPopulationSize());
	m_occupied.insert(index);
	return index;
}

std::vector<NNPPTrainingUpdate<float>> NNAITrainer::runSession(NeuronBuffer<float>& neuronBuffer) {
	std::vector<NNPPTrainingUpdate<float>> scoreUpdates;
	uint whitePlayerIndex = findAndStorePlayerIndex();
	uint blackPlayerIndex = findAndStorePlayerIndex();

	NNAI* white = m_trainee->getNNAiPtrAt(whitePlayerIndex);
	NNAI* black = m_trainee->getNNAiPtrAt(blackPlayerIndex);

	float pointsForWhite = runGameAgainstRandom(white, neuronBuffer);
	float pointsForBlack = runGameAgainstRandom(black, neuronBuffer);
	GameResult result = runGame(white, black, neuronBuffer);
	const float gamePoints = calculatePoints(white->getScore(), black->getScore());
	const float drawPoints = DRAW_POINTS * (gamePoints / POINTS_PER_GAME);

	switch (result) {
	case GameResult::WHITE_WINS:
		pointsForWhite += gamePoints + 1.0f;
		pointsForBlack += -gamePoints;
		break;
	case GameResult::BLACK_WINS:
		pointsForWhite += -gamePoints;
		pointsForBlack += gamePoints + 1.0f;
		break;
	case GameResult::DRAW:
	case GameResult::DRAW_NO_MOVES:
		pointsForWhite += white->getScore() > black->getScore() ? -drawPoints : drawPoints;
		pointsForBlack += white->getScore() > black->getScore() ? drawPoints : -drawPoints;
		break;
	default:
		break;
	case GameResult::WHITE_WINS_TIME:
	case GameResult::BLACK_WINS_TIME:
		assert(false);
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

uint NNAITrainer::sessionsTillEvolution() const {
	uint sessionsTillEvol = calculateSessionsToEvol();
	if (m_trainee->getSessionsTrainedThisGen() > sessionsTillEvol) {
		return 0;
	}
	return sessionsTillEvol - m_trainee->getSessionsTrainedThisGen();
}

GameResult NNAITrainer::runGame(const NNAI* white, const NNAI* black, NeuronBuffer<float>& neuronBuffer) const {
	ChessBoard b;
	NNAIPlayer whitePlayer(Color::WHITE, white, &neuronBuffer);
	NNAIPlayer blackPlayer(Color::BLACK, black, &neuronBuffer);
	Game g(b, &whitePlayer, &blackPlayer, MAX_MOVES_PER_GAME, false);
	return g.start(false);
}

float NNAITrainer::runGameAgainstRandom(const NNAI* ai, NeuronBuffer<float>& neuronBuffer) const {
	ChessBoard b;
	NNAIPlayer aiPlayer(Color::WHITE, ai, &neuronBuffer);
	RandomPlayer random(Color::BLACK);
	Game g(b, &aiPlayer, &random, MAX_MOVES_PER_RANDOM_GAME, false);
	return g.start(false) == GameResult::WHITE_WINS ? RANDOM_WIN_POINTS : 0.0f;
}
