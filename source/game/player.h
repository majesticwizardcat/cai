#pragma once

class Player;
class HumanPlayer;

#include "chess-board.h"
#include "game.h"
#include "tools/random-generator.h"

class Player {
protected:
	Color m_color;

public:
	Player() = delete;
	Player(Color color) : m_color(color) {} 
	virtual ~Player() { }

	constexpr Color getColor() const { return m_color; }
	virtual MoveResult getMove(const ChessBoard& board, BoardMove* move) = 0;
	virtual void revert() { };
};

class HumanPlayer : public Player {
public:
	HumanPlayer() = delete;
	HumanPlayer(Color color) : Player(color) { }

	MoveResult getMove(const ChessBoard& board, BoardMove* move);
};

class RandomPlayer : public Player {
public:
	RandomPlayer() = delete;
	RandomPlayer(Color color) : Player(color) { }

	MoveResult getMove(const ChessBoard& board, BoardMove* move);

private:
	RandomGenerator m_rgen;
};
