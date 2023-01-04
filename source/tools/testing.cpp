#include "tools/testing.h"
#include "game/chess-board.h"

namespace testing {

unsigned long long perft(const std::string& fen, unsigned int startingDepth, bool verbose) {
	std::function<unsigned long long(const ChessBoard&, int)> findPositions = [&](const ChessBoard& b, int depth) {
		MovesVector moves;
		b.getNextPlayerMoves(moves);
		if (depth == 1) {
			return static_cast<unsigned long long>(moves.size());
		}

		unsigned long long positions = 0;
		for (const auto& m : moves) {
			ChessBoard nextPosition(b);
			nextPosition.playMove(m);
			positions += findPositions(nextPosition, depth - 1);
		}
		return positions;
	};

	ChessBoard start(fen);

	if (verbose) {
		std::cout << "Starting perft test with depth " << startingDepth << "..." << '\n';
		start.printBoard();
	}

	unsigned long long positionsFound = 1;
	if (startingDepth > 0) {
		positionsFound = findPositions(start, startingDepth);
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