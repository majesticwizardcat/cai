#pragma once

class Player;
class HumanPlayer;

#include "board.h"

class Player {
protected:
	Color m_color;

public:
	Player() = delete;
	Player(Color color) : m_color(color) {} 

	inline Color getColor() const { return m_color; }
	virtual bool getMove(const Board& board, Move* move) const = 0;
};

class HumanPlayer : public Player {
public:
	HumanPlayer() = delete;
	HumanPlayer(Color color) : Player(color) { }

	bool getMove(const Board& board, Move* move) const;
};

