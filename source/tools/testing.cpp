#include "tools/testing.h"

namespace testing {

unsigned long long perft(const std::string& fen, unsigned int startingDepth, bool verbose) {
	std::function<unsigned long long(const ChessBoard&, int, Color)> findPositions = [&](const ChessBoard& b, int depth, Color nextPlayer) {
		MovesArray moves;
		unsigned int numberOfMoves = b.getMoves(nextPlayer, &moves);
		if (depth == 1) {
			return static_cast<unsigned long long>(numberOfMoves);
		}

		unsigned long long positions = 0;
		int curMoveIndex = 0;
		for (unsigned int i = 0; i < numberOfMoves; ++i) {
			const Move& m = moves[i];
			ChessBoard nextPosition(b);
			nextPosition.playMove(m);
			positions += findPositions(nextPosition, depth - 1, nextPlayer == Color::WHITE ? Color::BLACK : Color::WHITE);
		}
		return positions;
	};

	ChessBoard start;
	Color next = start.setPosition(fen);

	if (verbose) {
		std::cout << "Starting perft test with depth " << startingDepth << "..." << '\n';
		start.printBoard();
	}

	unsigned long long positionsFound = 1;
	if (startingDepth > 0) {
		positionsFound = findPositions(start, startingDepth, next);
	}

	if (verbose) {
		std::cout << "Positions: " << positionsFound << '\n';
	}
	return positionsFound;
}

unsigned long long perft(unsigned int depth) {
	return perft(DEFAULT_POSITION, depth, true);
}

}