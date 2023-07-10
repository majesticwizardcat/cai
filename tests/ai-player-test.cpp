#include "neural-net-ai/nnai-player.h"
#include "neural-net-ai/cai-population.h"
#include "neural-net-ai/nnai-trainer.h"

int main() {
	NNAI ai(0, CAI_LAYERS);
	auto neuronBuffer = allocNeuronBuffer<float>();
	ai.initRandomUniform(-1.0f, 1.0f);
	NNAIPlayer aiPlayer(Color::WHITE, &ai, 120, &neuronBuffer);
	ChessBoard board;
	BoardMove m;

	{
		board.printBoard();
		assert(aiPlayer.getMove(board, &m) == MoveResult::MOVE_OK);
		board.printMoveOnBoard(m);
		std::cout << '\n';
	}

	{
		board = ChessBoard("r4rk1/1pp1qppp/p1np1n2/2b1p1B1/2B1P1b1/P1NP1N2/1PP1QPPP/R4RK1 w - - 0 10");
		board.printBoard();
		assert(aiPlayer.getMove(board, &m) == MoveResult::MOVE_OK);
		board.printMoveOnBoard(m);
		std::cout << '\n';
	}

	{
		std::unique_ptr<CAIPopulation> pop = createCAIPopulation("ai-test.cai", 150);
		NNAITrainer trainer(13, 4, pop.get());
		trainer.run(true);
		pop->printInfo();
		std::cout << "Worst score: " << pop->getMinScoreNNAi().getScore() << '\n';
	}

	{
		std::unique_ptr<CAIPopulation> pop = std::make_unique<CAIPopulation>("ai-test.cai");
		pop->printInfo();
		std::cout << "Worst score: " << pop->getMinScoreNNAi().getScore() << '\n';
	}
}
