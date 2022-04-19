#pragma once

class Player;
class HumanPlayer;

#include "board.h"
#include "game.h"

class Player {
protected:
	Color m_color;

public:
	Player() = delete;
	Player(Color color) : m_color(color) {} 
	virtual ~Player() { }

	inline Color getColor() const { return m_color; }
	virtual MoveResult getMove(const ChessBoard& board, Move* move) = 0;
	virtual void revert() = 0;
};

class HumanPlayer : public Player {
public:
	HumanPlayer() = delete;
	HumanPlayer(Color color) : Player(color) { }

	MoveResult getMove(const ChessBoard& board, Move* move);
	void revert() { }
};

