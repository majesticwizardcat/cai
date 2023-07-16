#include "player.h"

#include <iostream>
#include <algorithm>

MoveResult HumanPlayer::getMove(const ChessBoard& board, BoardMove* move) {
	MovesVector moves;
	board.getMoves(m_color, moves);
	if (moves.empty()) {
		return MoveResult::OUT_OF_MOVES;
	}

	board.printBoard();
	for (int i = 0; i < moves.size(); ++i) {
		std::cout << i << ". ";
		board.printMoveOnBoard(moves[i]);
	}

	std::cout << "Choose move: ";
	int moveIndex = -1;
	std::string s;
	do {
		std::cin >> s;
		if (s == "revert") {
			return MoveResult::REVERT_REQUEST;
		}

		if (!s.empty() && std::find_if(s.begin(), s.end(),
			[](unsigned char c) { return !std::isdigit(c); }) == s.end()) {
			moveIndex = atoi(s.data());
		}
	} while (moveIndex < 0 || moveIndex > moves.size());

	*move = std::move(moves[moveIndex]);
	return MoveResult::MOVE_OK;
}

MoveResult RandomPlayer::getMove(const ChessBoard& board, BoardMove* move) {
	MovesVector moves;
	board.getMoves(m_color, moves);
	if (moves.empty()) {
		return MoveResult::OUT_OF_MOVES;
	}

	*move = moves[m_rgen.getUint32() % moves.size()];
	return MoveResult::MOVE_OK;
}
