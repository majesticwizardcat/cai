#include "min-max-ai/min-max-ai-player.h"

MoveResult MinMaxAiPlayer::getMove(const ChessBoard& board, BoardMove* move) {
	/*
	const auto getBetterEvalAndSwapIndices = [](Color color, ChessBoardEvalType newEval,
			ChessBoardEvalType currentBest, uint8_t newIndex, uint8_t* currentBestIndex) {
		// We are adding some randomness to same evaluated positions so the ai can have variaety 
		// This is also affected by what eval has each thread checked locally
		ChessBoardEvalType paddedNewEval = newEval;
		bool shouldSwap = (color == WHITE && paddedNewEval > currentBest) || (color == BLACK && paddedNewEval < currentBest);

		if (shouldSwap) {
			*currentBestIndex = *newIndex;
			return newEval;
		}
		return currentBest;
	};
	*/

	MovesVector moves;
	board.getMoves(m_color, moves);

	if (moves.empty()) {
		return MoveResult::OUT_OF_MOVES;
	}
	
	if (moves.size() == 1) {
		*move = moves[0];
		return MoveResult::MOVE_OK;
	}

	uint8_t bestMoveIndex = 0;
	int16_t eval = m_color == WHITE ? CHESS_BOARD_MIN_EVALUATION : CHESS_BOARD_MAX_EVALUATION;
	MinMaxTree minMaxTree;
	for (uint i = 0; i < moves.size(); ++i) {
		ChessBoard b(board);
		b.playMove(moves[i]);
		const int16_t positionEval = minMaxTree.expand(b, 7);

		b.printMoveOnBoard(moves[i]);
		std::cout << " -> Eval: " << positionEval << '\n';

		if ((m_color == WHITE && positionEval > eval) || (m_color == BLACK && positionEval < eval)) {
			bestMoveIndex = i;
			eval = positionEval;
		}
	}

	/*
	auto runEval = [&moves, &board, &bestMoveIndex, &eval, &getBetterEvalAndSwapIndices, this]() {
		MinMaxTreeChessBoard minMaxTree;
		uint8_t localBestMoveIndex = 0;
		ChessBoardEvalType localEval = m_color == WHITE ? CHESS_BOARD_MIN_EVALUATION : CHESS_BOARD_MAX_EVALUATION;
		while (m_nextMoveIndexAtomic < moves.size()) {
			uint8_t localNextMoveIndex = m_nextMoveIndexAtomic++;
			ChessBoard b(board);
			b.playMove(moves[localNextMoveIndex]);
			localEval = getBetterEvalAndSwapIndices(m_color, minMaxTree.expand(b, 5), localEval, &localNextMoveIndex, &localBestMoveIndex);
		}
		std::lock_guard<std::mutex> lock(m_evalMutex);
		eval = getBetterEvalAndSwapIndices(m_color, localEval, eval, &localBestMoveIndex, &bestMoveIndex);
	};

	std::vector<std::thread> threads;
	threads.reserve(m_numOfThreads);
	for (uint i = 0; i < m_numOfThreads; ++i) {
		threads.emplace_back(runEval);
	}

	for (auto& t : threads) {
		t.join();
	}
	*/

	*move = moves[bestMoveIndex];
	if (m_printEval) {
		std::cout << "Evaluation: " << eval << '\n';
	}
	return MoveResult::MOVE_OK;
}
