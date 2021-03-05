#pragma once

class Game;
enum class GameResult;

#include "game/player.h"
#include "game/board.h"

#include <utility>
#include <vector>

enum class GameResult { WHITE_WINS, BLACK_WINS, DRAW };

class Game {
private:
	Board m_board;
	const Player* m_white;
	const Player* m_black;
	const Player* m_current;
	std::vector<std::pair<const Player*, Board>> m_moves;

public:
	Game() = delete;
	Game(const Game& other) = delete;
	Game(const Board& board, const Player* white,
		const Player* black) : m_board(board), m_white(white),
		m_black(black), m_current(m_white) { }

	GameResult start();
	inline void printBoard() const { m_board.printBoard(); }
};

