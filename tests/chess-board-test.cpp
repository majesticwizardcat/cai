#include "game/chess-board.h"

int main() {
	ChessBoard board;
	board.printBoard();
	MovesVector moves;
	board.getMoves(WHITE, moves);
	assert(moves.size() == 20);
	moves.clear();
	board.getMoves(BLACK, moves);
	assert(moves.size() == 20);
	moves.clear();

	board = ChessBoard("rnbqkbnr/ppp1pppp/8/3p4/4P3/2N5/PPPP1PPP/R1BQKBNR b KQkq - 1 2");
	board.printBoard();
	board.getMoves(BLACK, moves);
	assert(moves.size() == 29);
	moves.clear();

	board = ChessBoard("8/2p5/K2p4/1P5r/1R3p1k/8/4P1P1/8 b - - 1 1");
	board.printBoard();
	board.getMoves(BLACK, moves);
	assert(moves.size() == 15);
	moves.clear();

	board = ChessBoard("8/2p5/3p4/1P5r/KR3p1k/8/4P1P1/8 b - - 1 1");
	board.printBoard();
	board.getMoves(BLACK, moves);
	assert(moves.size() == 15);
	moves.clear();

	board = ChessBoard("8/2p5/3p4/KP5r/5R1k/8/4P1P1/8 b - - 0 1");
	board.printBoard();
	board.getMoves(BLACK, moves);
	assert(moves.size() == 2);
	moves.clear();

	board = ChessBoard("8/2p5/3p4/KP5r/1R3pPk/8/4P3/8 b - - 0 1");
	board.printBoard();
	board.getMoves(BLACK, moves);
	assert(moves.size() == 17);
	moves.clear();

	board = ChessBoard("8/2p5/3p4/KP5r/1R3p1k/4P3/6P1/8 b - - 0 1");
	board.printBoard();
	board.getMoves(BLACK, moves);
	assert(moves.size() == 15);
	moves.clear();

	board = ChessBoard("8/2p5/3p4/KP5r/1R2Pp1k/8/6P1/8 b - - 0 1");
	board.printBoard();
	board.getMoves(BLACK, moves);
	assert(moves.size() == 16);
	moves.clear();

	board = ChessBoard("8/2p5/3p4/KP5r/5p1k/1R6/4P1P1/8 b - - 1 1");
	board.printBoard();
	board.getMoves(BLACK, moves);
	assert(moves.size() == 15);
	moves.clear();

	board = ChessBoard("8/2p5/3p4/KP5r/1R3p1k/6P1/4P3/8 b - - 0 1");
	board.printBoard();
	board.getMoves(BLACK, moves);
	assert(moves.size() == 4);
	moves.clear();

	board = ChessBoard("8/2p5/3p4/KP5r/5p1k/8/1R2P1P1/8 b - - 1 1");
	board.printBoard();
	board.getMoves(BLACK, moves);
	assert(moves.size() == 16);
	moves.clear();

	board = ChessBoard("8/2p5/3p4/KP5r/5p1k/8/4P1P1/1R6 b - - 1 1");
	board.printBoard();
	board.getMoves(BLACK, moves);
	assert(moves.size() == 16);
	moves.clear();

	board = ChessBoard("8/2p5/3p4/KP5r/2R2p1k/8/4P1P1/8 b - - 1 1");
	board.printBoard();
	board.getMoves(BLACK, moves);
	assert(moves.size() == 15);
	moves.clear();

	board = ChessBoard("8/2p5/3p4/KP5r/3R1p1k/8/4P1P1/8 b - - 1 1");
	board.printBoard();
	board.getMoves(BLACK, moves);
	assert(moves.size() == 15);
	moves.clear();

	board = ChessBoard("8/2p5/3p4/KP5r/R4p1k/8/4P1P1/8 b - - 1 1");
	board.printBoard();
	board.getMoves(BLACK, moves);
	assert(moves.size() == 15);
	moves.clear();

	board = ChessBoard("8/2p5/3p4/KP5r/4Rp1k/8/4P1P1/8 b - - 1 1");
	board.printBoard();
	board.getMoves(BLACK, moves);
	assert(moves.size() == 15);
	moves.clear();

	board = ChessBoard("8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - ");
	board.printBoard();
	board.getMoves(WHITE, moves);
	assert(moves.size() == 14);
	uint32_t moveCount = 0;
	for (const auto& move : moves) {
		MovesVector newMoves;
		ChessBoard newBoard(board);
		newBoard.playMove(move);
		newBoard.getMoves(BLACK, newMoves);
		moveCount += newMoves.size();
	}
	assert(moveCount == 191);
}
