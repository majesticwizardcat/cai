#include "tools/testing.h"
#include "game/chess-board.h"
#include "unordered_dense.h"

#include <thread>
#include <mutex>

namespace testing {

uint64_t perft(const std::string& fen, uint8_t startingDepth, bool verbose) {
	typedef ankerl::unordered_dense::map<ChessBoard, uint64_t, BoardHasher> MemoMap;

	const auto findPositionsInternal = [](const ChessBoard& b, uint8_t depth, std::vector<MemoMap>& memos, const auto& funcRef) {
		auto it = memos[depth].find(b);
		if (it != memos[depth].end()) {
			return it->second;
		}

		MovesVector moves;
		b.getNextPlayerMoves(moves);
		if (depth == 1) {
			return static_cast<uint64_t>(moves.size());
		}

		uint64_t positions = 0;
		for (const auto& m : moves) {
			ChessBoard nextPosition(b);
			nextPosition.playMove(m);
			positions += funcRef(nextPosition, depth - 1, memos, funcRef);
		}
		memos[depth][b] = positions;
		return positions;
	};

	ChessBoard startingPosition(fen);
	uint64_t numOfThreads = std::thread::hardware_concurrency();
	numOfThreads = 1;

	if (verbose) {
		std::cout << "Starting perft test with depth " << startingDepth << " using " << numOfThreads << " threads..." << '\n';
		startingPosition.printBoard();
	}

	uint64_t positionsFound = 1;
	std::vector<ChessBoard> positionsToCheck;
	std::mutex positionsMutex;
	std::mutex countMutex;

	const auto runThread = [&findPositionsInternal, &positionsFound, &positionsToCheck, &positionsMutex, &countMutex](uint8_t depth) {
		uint64_t positionsFoundForThread = 0;
		std::vector<MemoMap> memos(depth + 1);
		ChessBoard nextPosition;
		while (true) {
			{
				std::lock_guard<std::mutex> lock(positionsMutex);
				if (positionsToCheck.empty()) {
					break;
				}
				nextPosition = positionsToCheck.back();
				positionsToCheck.pop_back();
			}
			positionsFoundForThread += findPositionsInternal(nextPosition, depth, memos, findPositionsInternal);
		}

		std::lock_guard<std::mutex> lock(countMutex);
		positionsFound += positionsFoundForThread;
	};

	if (startingDepth > 0) {
		positionsFound = 0;
		{
			MovesVector moves;
			startingPosition.getNextPlayerMoves(moves);
			if (startingDepth == 1) {
				return moves.size();
			}

			for (const auto& m : moves) {
				positionsToCheck.push_back(startingPosition);
				positionsToCheck.back().playMove(m);
			}
		}
		std::vector<std::thread> threads;
		for (uint32_t t = 0; t < numOfThreads; t++) {
			threads.emplace_back(runThread, startingDepth - 1);
		}

		for (auto& t : threads) {
			t.join();
		}
	}

	if (verbose) {
		std::cout << "Positions: " << positionsFound << '\n';
	}
	return positionsFound;
}

uint64_t perft(uint8_t depth) {
	return perft(DEFAULT_POSITION, depth, true);
}

}