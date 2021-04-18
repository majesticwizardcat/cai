#include "game/game.h"

#include <iostream>

GameResult Game::start(bool verbose) {
	Move m;
	while (m_maxMoves == 0 || (m_board.movesPlayed() < m_maxMoves)) {
		m_moves.push_back(std::make_pair(m_current, m_board));
		if (!m_current->getMove(m_board, &m)) {
			if (m_board.getMoves(m_current->getColor()).size() > 0
				|| m_board.isKingInCheck(m_current->getColor())) {
				return m_current == m_white ? GameResult::WHITE_WINS
					: GameResult::BLACK_WINS;
			}
			return GameResult::DRAW;
		}
		m_board.playMove(m);
		if (m_current == m_white) {
			m_current = m_black;
		}
		else {
			m_current = m_white;
		}
		if (verbose) {
			std::cout << "Played: ";
			m.printMove();
			std::cout << '\n';
		}
	}
	return GameResult::DRAW;
}

