#include "game/board.h"

#include <iostream>

const char* TEST0 = "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq -";

int main() {
	Board b;
	b.setPosition(TEST0);
	b.printBoard();
	auto whiteCastles = b.canCastle(Color::WHITE);
	auto blackCastles = b.canCastle(Color::WHITE);
	if (whiteCastles.first && whiteCastles.second
		&& blackCastles.first && blackCastles.second) {
		std::cout << "Castles correct!" << '\n';
	}
	else {
		std::cout << "Castles failed!" << '\n';
	}
}

