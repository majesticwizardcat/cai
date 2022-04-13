#include "ai/ai-player.h"
#include "ai/cai-population.h"
#include "ai/ai-trainer.h"

#include <chrono>

const uint TESTS = MAX_MOVES * 2;

int main() {
	assert(false); // DONT RUN PERFORMANCE TESTS ON DEBUG
	AI ai(CAI_LAYERS);
	ai.initRandomUniform(-1.0f, 1.0f);
	ChessBoard board;
	board.setPosition("r4rk1/1pp1qppp/p1np1n2/2b1p1B1/2B1P1b1/P1NP1N2/1PP1QPPP/R4RK1 w - - 0 10");

	{
		std::cout << "Running " << TESTS << " move fetches from ai with 2 seconds cycles per move" << '\n';
		AIPlayer aiPlayer(Color::WHITE, &ai, 0, 2 * CYCLES_PER_SECOND);
		Move m;
		auto start = std::chrono::high_resolution_clock::now();
		for (uint i = 0; i < TESTS; ++i) {
			ai.initRandomUniform(-1.0f, 1.0f);
			aiPlayer.getMove(board, &m);
		}
		std::chrono::duration<float> duration = std::chrono::high_resolution_clock::now() - start;
		std::cout << "Done! Time taken: " << duration.count() << " seconds" <<  '\n';
	}

	{
		std::cout << "Running depleting cycles test for 3 hours of cycles" << '\n';
		AIPlayer aip(Color::WHITE, &ai, CYCLES_PER_HOUR * 3, 0);
		Move m;
		auto start = std::chrono::high_resolution_clock::now();
		uint movesPlayed = 0;
		while (aip.getMove(board, &m)) {
			ai.initRandomUniform(-1.0f, 1.0f);
			movesPlayed++;
		}
		std::chrono::duration<float> duration = std::chrono::high_resolution_clock::now() - start;
		std::cout << "Done! Time taken: " << duration.count() << " seconds, " << " moves played: " << movesPlayed <<  '\n';
	}

	{
		std::cout << "Running " << TESTS << " games of depleting cycles test for 10 minutes of cycles each" << '\n';
		Move m;
		auto start = std::chrono::high_resolution_clock::now();
		uint movesPlayed = 0;

		for (uint i = 0; i < TESTS; ++i) {
			AIPlayer aip(Color::WHITE, &ai, CYCLES_PER_MINUTE * 10, 0);
			while (aip.getMove(board, &m)) {
				ai.initRandomUniform(-1.0f, 1.0f);
				movesPlayed++;
			}
		}

		std::chrono::duration<float> duration = std::chrono::high_resolution_clock::now() - start;
		std::cout << "Done! Time taken: " << duration.count() << " seconds, moves played: " << movesPlayed <<  '\n';
	}

	{
		std::cout << "Running all training games till depletion" << '\n';
		Move m;
		auto start = std::chrono::high_resolution_clock::now();
		uint movesPlayed = 0;

		for (const auto& test : GAMES) {
			ai.initRandomUniform(-1.0f, 1.0f);
			AIPlayer aip(Color::WHITE, &ai, test.totalCycles, test.cyclesPerMove);
			if (test.cyclesPerMove > 0) {
				for (uint i = 0; i < TESTS; ++i) {
					aip.getMove(board, &m);
				}
				movesPlayed += TESTS;
			}
			else {
				while (aip.getMove(board, &m)) {
					movesPlayed++;
				}
			}
		}

		std::chrono::duration<float> duration = std::chrono::high_resolution_clock::now() - start;
		std::cout << "Done! Time taken: " << duration.count() << " seconds, moves played: " << movesPlayed <<  '\n';
	}

	{
		const uint sessions = 200;
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
