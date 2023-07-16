#pragma once

class Game;

enum class GameResult { WHITE_WINS, BLACK_WINS, WHITE_WINS_TIME, BLACK_WINS_TIME, DRAW, DRAW_NO_MOVES };
enum class MoveResult { MOVE_OK, OUT_OF_MOVES, OUT_OF_TIME, REVERT_REQUEST };

#include "game/player.h"
#include "game/chess-board.h"

#include <utility>
#include <vector>

class Game {
public:
	Game() = delete;
	Game(const Game& other) = delete;
	Game(const ChessBoard& board, Player* white, Player* black, int maxMoves, bool storeMoves)
		: m_board(board)
		, m_white(white)
		, m_black(black)
		, m_current(m_white)
		, m_numMovesPlayed(0)
		, m_maxMoves(maxMoves)
		, m_storeMoves(storeMoves) { }

	GameResult start(bool verbose);
	inline void printBoard() const { m_board.printBoard(); }

private:
	ChessBoard m_board;
	Player* m_white;
	Player* m_black;
	Player* m_current;
	std::vector<ChessBoard> m_boardStates;
	uint m_numMovesPlayed;
	uint m_maxMoves;
	bool m_storeMoves;

	void nextPlayer();
	void playMove(const BoardMove& move, bool verbose);
	void revert();
};
