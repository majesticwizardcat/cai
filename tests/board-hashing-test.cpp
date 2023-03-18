#include "tools/board-hashing.hpp"
#include "game/chess-board.h"
#include "tools/random-generator.h"

static uint32_t TEST_RUNS = 1000000000;

int main() {
	ChessBoard board;
	RandomGenerator rgen;

	MovesVector moves;
	uint32_t timesReseted = 0;
	for (uint32_t i = 0; i < TEST_RUNS; ++i) {
		moves.clear();
		board.getNextPlayerMoves(moves);
		if (moves.empty() || board.isDraw()) {
			board = ChessBoard();
			timesReseted++;
			continue;
		}
		board.playMove(moves[rgen.getUint32() % moves.size()]);
		uint64_t currentHash = board.getHash();
		board.calculateHashFromCurrentState();
		if (currentHash != board.getHash()) {
			std::cout << "Test failed!" << '\n';
			exit(0);
		}
		std::cout << "\rTests done: " << (i + 1) << " out of " << TEST_RUNS;
	}
	std::cout << "\nDone! All tests passed! Times reset: " << timesReseted << '\n';
}
