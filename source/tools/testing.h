#pragma once

#include "game/board.h"

#include <string>
#include <iostream>
#include <functional>

const std::string DEFAULT_POSITION = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";

namespace testing {
	unsigned long long perft(const std::string& fen, unsigned int startingDepth, bool verbose);
	unsigned long long perft(unsigned int depth);
}
