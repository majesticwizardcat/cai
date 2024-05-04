#include "min-max-ai/min-max-ai-player.h"

#include <iostream>
#include <chrono>

const uint32_t TESTS = 100;

int main() {
	{
		ChessBoard board("r1bqkbnr/1ppppQpp/2n5/p7/2B1P3/8/PPPP1PPP/RNB1K1NR b KQkq - 0 4");
		MovesVector moves;
		board.getMoves(board.getNextPlayerColor(), moves);
		assert(evaluate(board, moves) == CHESS_BOARD_MAX_EVALUATION);
	}
	{
		ChessBoard board("rnb1k1nr/pppp1ppp/8/2b1p3/P7/8/1PPPPqPP/RNBQKBNR w KQkq - 0 5");
		MovesVector moves;
		board.getMoves(board.getNextPlayerColor(), moves);
		assert(evaluate(board, moves) == CHESS_BOARD_MIN_EVALUATION);
	}

	// Check a position
//	ChessBoard board("rnbqkbnr/1ppppppp/8/p7/2B1P3/5Q2/PPPP1PPP/RNB1K1NR b KQkq - 1 3");
	ChessBoard board("r1bqk2r/1pp1bpp1/2n1p1n1/3p3p/p2PP2P/2PBBQ2/PP1N1PP1/2KR2NR w kq - 0 11");

	MinMaxAiPlayer minMaxPlayer(board.getNextPlayerColor(), true, false, 1);

	BoardMove move;
	minMaxPlayer.getMove(board, &move);

	board.printBoard();
	board.printMoveOnBoard(move);

	/*
	// Run some tests to check performance
	std::chrono::time_point start = std::chrono::high_resolution_clock::now();
	ChessBoard board;
	BoardMove m;
	for (uint32_t i = 0; i < TESTS; ++i) {
		MinMaxAiPlayer pl(board.getNextPlayerColor(), false, false, 1);
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
	*/

/*
	// Run a human vs AI game
	std::cout << "Starting game: " << '\n';
	ChessBoard board;

	MinMaxAiPlayer white(BLACK, true, true);
	HumanPlayer black(WHITE);
	Game g(board, &black, &white, 0, true);

	g.start(true);
	*/
}
