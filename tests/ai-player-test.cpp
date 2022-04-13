#include "ai/ai-player.h"
#include "ai/cai-population.h"
#include "ai/ai-trainer.h"

int main() {
	AI ai(CAI_LAYERS);
	ai.initRandomUniform(-1.0f, 1.0f);
	AIPlayer aiPlayer(Color::WHITE, &ai, 0, 120);
	ChessBoard board;
	Move m;

	{
		board.setupBoard();
		board.printBoard();
		assert(aiPlayer.getMove(board, &m));
		m.printMove();
		std::cout << '\n';
	}

	{
		board.setPosition("r4rk1/1pp1qppp/p1np1n2/2b1p1B1/2B1P1b1/P1NP1N2/1PP1QPPP/R4RK1 w - - 0 10");
		board.printBoard();
		assert(aiPlayer.getMove(board, &m));
		m.printMove();
		std::cout << '\n';
	}

	{
		std::unique_ptr<CAIPopulation> pop = createCAIPopulation("ai-test.cai", 150);
		AITrainer trainer(500, 4, pop.get());
		trainer.run(true);
		pop->printInfo();
		std::cout << "Best score: " << pop->getBestNNAiConstRef().getScore() << '\n';
		std::cout << "Worst score: " << pop->getMinScoreNNAi().getScore() << '\n';
		pop->saveToDisk("ai-test.cai");
	}

	{
		std::unique_ptr<CAIPopulation> pop = std::make_unique<CAIPopulation>("ai-test.cai");
		pop->printInfo();
		std::cout << "Best score: " << pop->getBestNNAiConstRef().getScore() << '\n';
		std::cout << "Worst score: " << pop->getMinScoreNNAi().getScore() << '\n';
	}
}
