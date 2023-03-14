#pragma once

#include <string>
#include <iostream>
#include <functional>

const std::string DEFAULT_POSITION = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";

namespace testing {
	uint64_t perft(const std::string& fen, uint8_t startingDepth, bool verbose);
	uint64_t perft(uint8_t startingDepth);
}
