#include "ai/ai-player.h"
#include "ai/cai-population.h"
#include "ai/ai-trainer.h"

#include <chrono>

const uint TESTS = 10;

int main() {
	//assert(false); // DONT RUN PERFORMANCE TESTS ON DEBUG
	AI ai(0, CAI_LAYERS);
	ai.initRandomUniform(MIN_AI_WEIGHT_VALUE, MAX_AI_WEIGHT_VALUE);
	ChessBoard board;
	board.setPosition("r4rk1/1pp1qppp/p1np1n2/2b1p1B1/2B1P1b1/P1NP1N2/1PP1QPPP/R4RK1 w - - 0 10");

	{
		std::cout << "Running " << TESTS << " move fetches from ai with 2 seconds cycles per move" << '\n';
		AIPlayer aiPlayer(Color::WHITE, &ai, 2 * CYCLES_PER_SECOND);
		Move m;
		auto start = std::chrono::high_resolution_clock::now();
		for (uint i = 0; i < TESTS; ++i) {
			ai.initRandomUniform(MIN_AI_WEIGHT_VALUE, MAX_AI_WEIGHT_VALUE);
			aiPlayer.getMove(board, &m);
		}
		std::chrono::duration<float> duration = std::chrono::high_resolution_clock::now() - start;
		std::cout << "Done! Time taken: " << duration.count() << " seconds" <<  '\n';
	}

	{
		const uint sessions = 100;
		const uint threads = 4;
		const uint size = 100;
		std::cout << "Running a trainer with populatio " << size << " test for " << sessions << " sessions with " << threads << " threads" << '\n';
		std::unique_ptr<CAIPopulation> pop = createCAIPopulation("perft-test-pop.cai", size);
		AITrainer trainer(sessions, threads, pop.get());

		auto start = std::chrono::high_resolution_clock::now();

		trainer.run(true);

		std::chrono::duration<float> duration = std::chrono::high_resolution_clock::now() - start;
		std::cout << "Done! Time taken: " << duration.count() << " seconds" << '\n';
	}
}
