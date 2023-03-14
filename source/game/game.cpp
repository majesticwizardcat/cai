#include "game/game.h"

#include <iostream>

GameResult Game::start(bool verbose) {
	BoardMove m;

	while (m_maxMoves <= 0 || m_boardStates.size() < m_maxMoves) {
		if (m_board.isDraw()) {
			return GameResult::DRAW;
		}

		switch(m_current->getMove(m_board, &m)) {
		case MoveResult::MOVE_OK:
			playMove(m, verbose);
			break;
		case MoveResult::OUT_OF_MOVES:
			if (m_board.isKingInCheck(m_current->getColor())) { // King is in check and no moves -> Checkmate
				return m_current == m_white ? GameResult::BLACK_WINS : GameResult::WHITE_WINS;
			}
			else { // No moves but king is not in check -> Stalemate
				return GameResult::DRAW;
			}
		case MoveResult::OUT_OF_TIME:
			return  m_current == m_white ? GameResult::BLACK_WINS_TIME : GameResult::WHITE_WINS_TIME;
		case MoveResult::REVERT_REQUEST:
			revert();
			break;
		}
	}

	return GameResult::DRAW_NO_MOVES;
}

void Game::nextPlayer() {
	if (m_current == m_white) {
		m_current = m_black;
	}
	else {
		m_current = m_white;
	}
}

void Game::playMove(const BoardMove& move, bool verbose) {
	if (verbose) {
		std::cout << "Played: ";
		m_board.printMoveOnBoard(move);
		std::cout << '\n';
	}
	if (m_storeMoves) {
		m_boardStates.push_back(m_board);
	}
	m_board.playMove(move);

	nextPlayer();
}

void Game::revert() {
	if (!m_storeMoves || m_boardStates.size() < 1) {
		return;
	}

	m_white->revert();
	m_black->revert();
	m_boardStates.pop_back();
	m_board = m_boardStates.back();
	m_boardStates.pop_back();
}
