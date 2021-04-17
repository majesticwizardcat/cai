#include "ai/ai.h"
#include "ai/ai-player.h"
#include "game/game.h"

int main() {
	AI ai;
	AIPlayer aiPlayer(Color::BLACK, &ai);
	HumanPlayer player(Color::WHITE);
	Board b;
	b.setupBoard();
	Game game(b, &player, &aiPlayer);
	game.start();
}

