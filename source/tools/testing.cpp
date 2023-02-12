#include "tools/testing.h"
#include "game/chess-board.h"

#include <unordered_map>

namespace testing {

unsigned long long perft(const std::string& fen, uint64_t startingDepth, bool verbose) {
	typedef std::unordered_map<ChessBoard, uint64_t, BoardHasher> MemoMap;
	std::vector<MemoMap> memos(startingDepth + 1);
	std::function<uint64_t(const ChessBoard&, uint64_t)> findPositions = [&](const ChessBoard& b, uint64_t depth) {
		auto it = memos[depth].find(b);
		if (it != memos[depth].end()) {
			return it->second;
		}

		MovesVector moves;
		b.getNextPlayerMoves(moves);
		if (depth == 1) {
			return moves.size();
		}

		uint64_t positions = 0;
		for (const auto& m : moves) {
			ChessBoard nextPosition(b);
			nextPosition.playMove(m);
			positions += findPositions(nextPosition, depth - 1);
		}
		memos[depth][b] = positions;
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

unsigned long long perft(uint64_t depth) {
	return perft(DEFAULT_POSITION, depth, true);
}

}