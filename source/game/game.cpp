#include "game/game.h"

#include <iostream>

GameResult Game::start(bool verbose, bool saveMoves) {
	Move m;
	GameResult result = GameResult::DRAW;
	while (m_maxMoves <= 0 || m_board.movesPlayed() < m_maxMoves) {
		if (saveMoves) {
			m_moves.push_back(std::make_pair(m_current, m_board));
		}

		if (m_board.isDraw()) {
			break;
		}

		if (!m_current->getMove(m_board, &m)) {
			MovesArray moves;
			if (m_board.getMoves(m_current->getColor(), &moves) > 0 // Still have moves but no time left
				|| m_board.isKingInCheck(m_current->getColor())) { // King is in check and no moves -> Checkmate
				result =  m_current == m_white ? GameResult::BLACK_WINS : GameResult::WHITE_WINS;
			}
			break;
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
	return result;
}
