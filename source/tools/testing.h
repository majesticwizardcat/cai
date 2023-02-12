#pragma once

#include <string>
#include <iostream>
#include <functional>

const std::string DEFAULT_POSITION = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";

namespace testing {
	unsigned long long perft(const std::string& fen, uint64_t startingDepth, bool verbose);
	unsigned long long perft(uint64_t startingDepth);
}
