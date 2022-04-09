#include "tools/testing.h"

namespace testing {

unsigned long long perft(const std::string& fen, unsigned int startingDepth, bool verbose) {
	std::function<unsigned long long(const ChessBoard&, int, Color)> findPositions = [&](const ChessBoard& b, int depth, Color nextPlayer) {
		MovesStackVector moves;
		b.getMoves(nextPlayer, &moves);
		if (depth == 1) {
			return static_cast<unsigned long long>(moves.size());
		}

		unsigned long long positions = 0;
		int curMoveIndex = 0;
		for (const auto& m : moves) {
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