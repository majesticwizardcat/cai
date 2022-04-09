#include "player.h"

#include <iostream>
#include <algorithm>

bool HumanPlayer::getMove(const ChessBoard& board, Move* move) {
	MovesStackVector moves;
	board.getMoves(m_color, &moves);
	if (moves.empty()) {
		return false;
	}

	board.printBoard();
	for (int i = 0; i < moves.size(); ++i) {
		std::cout << i << ". ";
		moves[i].printMove();
		std::cout << '\n';
	}

	std::cout << "Choose move: ";
	int moveIndex = -1;
	std::string s;
	do {
		std::cin >> s;
		if (!s.empty() && std::find_if(s.begin(), s.end(),
			[](unsigned char c) { return !std::isdigit(c); }) == s.end()) {
			moveIndex = atoi(s.data());
		}
	} while (moveIndex < 0 || moveIndex > moves.size());

	*move = std::move(moves[moveIndex]);
	return true;
}

