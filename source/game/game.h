#pragma once

class Game;

enum class GameResult { WHITE_WINS, BLACK_WINS, DRAW };

#include "game/player.h"
#include "game/board.h"

#include <utility>
#include <vector>

class Game {
private:
	Board m_board;
	Player* m_white;
	Player* m_black;
	Player* m_current;
	std::vector<std::pair<const Player*, Board>> m_moves;

public:
	Game() = delete;
	Game(const Game& other) = delete;
	Game(const Board& board, Player* white,
		Player* black) : m_board(board), m_white(white),
		m_black(black), m_current(m_white) { }

	GameResult start() { return start(true); }
	GameResult start(bool verbose);
	inline void printBoard() const { m_board.printBoard(); }
};

