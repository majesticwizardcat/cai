#include "min-max-ai/min-max-ai-player.h"

#include <iostream>
#include <chrono>

const uint32_t TESTS = 15;

int main() {
	// Check a position
	/*
	ChessBoard board("rnbqkbnr/1ppppppp/8/p7/2B1P3/5Q2/PPPP1PPP/RNB1K1NR b KQkq - 1 3");
	
	MinMaxAiPlayer minMaxPlayer(board.getNextPlayerColor(), true, false);

	BoardMove move;
	minMaxPlayer.getMove(board, &move);

	board.printBoard();
	board.printMoveOnBoard(move);
	*/
	// Run some tests to check performance
	std::chrono::time_point start = std::chrono::high_resolution_clock::now();
	ChessBoard board;
	BoardMove m;
	for (uint32_t i = 0; i < TESTS; ++i) {
		MinMaxAiPlayer pl(board.getNextPlayerColor(), false, false);
		MoveResult res = pl.getMove(board, &m);
		if (res == MoveResult::MOVE_OK && !board.isDraw()) {
			board.playMove(m);
		}
		else {
			board = ChessBoard();
		}
		std::cout << "\rTests ran: " << i + 1 << " out of: " << TESTS;
		std::cout.flush();
	}
	std::cout << '\n';

	std::chrono::duration<float> duration = std::chrono::high_resolution_clock::now() - start;
	std::cout << "Time: " << duration.count() << '\n';

	// Run a human vs AI game
/*
	std::cout << "Starting game: " << '\n';
	ChessBoard board;

	MinMaxAiPlayer white(BLACK, true, true);
	HumanPlayer black(WHITE);
	Game g(board, &black, &white, 0, true);

	g.start(true);
	*/
}
