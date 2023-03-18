#include "game/chess-board.h"
#include "tools/testing.h"
#include "tools/board-hashing.hpp"

#include <new>
#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <chrono>

unsigned long long allocs = 0;
unsigned long long totalAllocatedMem = 0;

void* operator new(std::size_t size) {
	allocs++;
	totalAllocatedMem += size;
	void *p = malloc(size);
	if (!p) throw std::bad_alloc();
	return p;
}

void* operator new[](std::size_t size) {
	allocs++;
	totalAllocatedMem += size;
	void *p = malloc(size);
	if (!p) throw std::bad_alloc();
	return p;
}

void* operator new[](std::size_t size, const std::nothrow_t&) throw() {
	allocs++;
	return malloc(size);
}

void* operator new(std::size_t size, const std::nothrow_t&) throw() {
	allocs++;
	return malloc(size);
}

void operator delete(void* ptr) throw() { free(ptr); }
void operator delete(void* ptr, const std::nothrow_t&) throw() { free(ptr); }
void operator delete[](void* ptr) throw() { free(ptr); }
void operator delete[](void* ptr, const std::nothrow_t&) throw() { free(ptr); }

struct PerftTest {
public:
	std::string fen;
	unsigned int depth;
	unsigned long long expected;

	PerftTest(const std::string& fen, unsigned int depth, unsigned long long expected)
		: fen(fen)
		, depth(depth)
		, expected(expected) { }
};

const std::vector<PerftTest> TESTS = {
	PerftTest("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1", 0, 1),
	PerftTest("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1", 1, 20),
	PerftTest("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1", 2, 400),
	PerftTest("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1", 3, 8902),
	PerftTest("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1", 4, 197281),
	PerftTest("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1", 5, 4865609),
	PerftTest("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1", 6, 119060324),
	PerftTest("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1", 7, 3195901860),
	PerftTest("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1", 8, 84998978956),
	//PerftTest("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1", 9, 2439530234167), // This is commented out since it takes a long time to compute

	PerftTest("r4rk1/1pp1qppp/p1np1n2/2b1p1B1/2B1P1b1/P1NP1N2/1PP1QPPP/R4RK1 w - - 0 10", 1, 46),
	PerftTest("r4rk1/1pp1qppp/p1np1n2/2b1p1B1/2B1P1b1/P1NP1N2/1PP1QPPP/R4RK1 w - - 0 10", 2, 2079),
	PerftTest("r4rk1/1pp1qppp/p1np1n2/2b1p1B1/2B1P1b1/P1NP1N2/1PP1QPPP/R4RK1 w - - 0 10", 3, 89890),
	PerftTest("r4rk1/1pp1qppp/p1np1n2/2b1p1B1/2B1P1b1/P1NP1N2/1PP1QPPP/R4RK1 w - - 0 10", 4, 3894594),
	PerftTest("r4rk1/1pp1qppp/p1np1n2/2b1p1B1/2B1P1b1/P1NP1N2/1PP1QPPP/R4RK1 w - - 0 10", 5, 164075551),
	PerftTest("r4rk1/1pp1qppp/p1np1n2/2b1p1B1/2B1P1b1/P1NP1N2/1PP1QPPP/R4RK1 w - - 0 10", 6, 6923051137),

	PerftTest("rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ - 1 8", 1, 44),
	PerftTest("rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ - 1 8", 2, 1486),
	PerftTest("rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ - 1 8", 3, 62379),
	PerftTest("rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ - 1 8", 4, 2103487),
	PerftTest("rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ - 1 8", 5, 89941194),

	PerftTest("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq -", 1, 48),
	PerftTest("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq -", 2, 2039),
	PerftTest("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq -", 3, 97862),
	PerftTest("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq -", 4, 4085603),
	PerftTest("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq -", 5, 193690690),
	PerftTest("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq -", 6, 8031647685),

	PerftTest("8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - ", 1, 14),
	PerftTest("8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - ", 2, 191),
	PerftTest("8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - ", 3, 2812),
	PerftTest("8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - ", 4, 43238),
	PerftTest("8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - ", 5, 674624),
	PerftTest("8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - ", 6, 11030083),
	PerftTest("8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - ", 7, 178633661),

	PerftTest("r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1RK1 w kq - 0 1", 1, 6),
	PerftTest("r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1RK1 w kq - 0 1", 2, 264),
	PerftTest("r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1RK1 w kq - 0 1", 3, 9467),
	PerftTest("r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1RK1 w kq - 0 1", 4, 422333),
	PerftTest("r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1RK1 w kq - 0 1", 5, 15833292),
	PerftTest("r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1RK1 w kq - 0 1", 6, 706045033),

	PerftTest("r2q1rk1/pP1p2pp/Q4n2/bbp1p3/Np6/1B3NBn/pPPP1PPP/R3K2R b KQ - 0 1", 1, 6),
	PerftTest("r2q1rk1/pP1p2pp/Q4n2/bbp1p3/Np6/1B3NBn/pPPP1PPP/R3K2R b KQ - 0 1", 2, 264),
	PerftTest("r2q1rk1/pP1p2pp/Q4n2/bbp1p3/Np6/1B3NBn/pPPP1PPP/R3K2R b KQ - 0 1", 3, 9467),
	PerftTest("r2q1rk1/pP1p2pp/Q4n2/bbp1p3/Np6/1B3NBn/pPPP1PPP/R3K2R b KQ - 0 1", 4, 422333),
	PerftTest("r2q1rk1/pP1p2pp/Q4n2/bbp1p3/Np6/1B3NBn/pPPP1PPP/R3K2R b KQ - 0 1", 5, 15833292),

	PerftTest("8/p7/8/1P6/K1k3p1/6P1/7P/8 w - -", 1, 5),
	PerftTest("8/p7/8/1P6/K1k3p1/6P1/7P/8 w - -", 2, 39),
	PerftTest("8/p7/8/1P6/K1k3p1/6P1/7P/8 w - -", 3, 237),
	PerftTest("8/p7/8/1P6/K1k3p1/6P1/7P/8 w - -", 4, 2002),
	PerftTest("8/p7/8/1P6/K1k3p1/6P1/7P/8 w - -", 5, 14062),
	PerftTest("8/p7/8/1P6/K1k3p1/6P1/7P/8 w - -", 6, 120995),
	PerftTest("8/p7/8/1P6/K1k3p1/6P1/7P/8 w - -", 7, 966152),
	PerftTest("8/p7/8/1P6/K1k3p1/6P1/7P/8 w - -", 8, 8103790),

	PerftTest("r3k2r/p6p/8/B7/1pp1p3/3b4/P6P/R3K2R w KQkq -", 1, 17),
	PerftTest("r3k2r/p6p/8/B7/1pp1p3/3b4/P6P/R3K2R w KQkq -", 2, 341),
	PerftTest("r3k2r/p6p/8/B7/1pp1p3/3b4/P6P/R3K2R w KQkq -", 3, 6666),
	PerftTest("r3k2r/p6p/8/B7/1pp1p3/3b4/P6P/R3K2R w KQkq -", 4, 150072),
	PerftTest("r3k2r/p6p/8/B7/1pp1p3/3b4/P6P/R3K2R w KQkq -", 5, 3186478),
	PerftTest("r3k2r/p6p/8/B7/1pp1p3/3b4/P6P/R3K2R w KQkq -", 6, 77054993),

	PerftTest("8/5p2/8/2k3P1/p3K3/8/1P6/8 b - -", 1, 9),
	PerftTest("8/5p2/8/2k3P1/p3K3/8/1P6/8 b - -", 2, 85),
	PerftTest("8/5p2/8/2k3P1/p3K3/8/1P6/8 b - -", 3, 795),
	PerftTest("8/5p2/8/2k3P1/p3K3/8/1P6/8 b - -", 4, 7658),
	PerftTest("8/5p2/8/2k3P1/p3K3/8/1P6/8 b - -", 5, 72120),
	PerftTest("8/5p2/8/2k3P1/p3K3/8/1P6/8 b - -", 6, 703851),
	PerftTest("8/5p2/8/2k3P1/p3K3/8/1P6/8 b - -", 7, 6627106),
	PerftTest("8/5p2/8/2k3P1/p3K3/8/1P6/8 b - -", 8, 64451405),

	PerftTest("r3k2r/pb3p2/5npp/n2p4/1p1PPB2/6P1/P2N1PBP/R3K2R b KQkq -", 1, 29),
	PerftTest("r3k2r/pb3p2/5npp/n2p4/1p1PPB2/6P1/P2N1PBP/R3K2R b KQkq -", 2, 953),
	PerftTest("r3k2r/pb3p2/5npp/n2p4/1p1PPB2/6P1/P2N1PBP/R3K2R b KQkq -", 3, 27990),
	PerftTest("r3k2r/pb3p2/5npp/n2p4/1p1PPB2/6P1/P2N1PBP/R3K2R b KQkq -", 4, 909807),

	PerftTest("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1", 1, 48),
	PerftTest("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1", 2, 2039),
	PerftTest("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1", 3, 97862),
	PerftTest("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1", 4, 4085603),
	PerftTest("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1", 5, 193690690),

	PerftTest("n1n5/PPPk4/8/8/8/8/4Kppp/5N1N b - - 0 1", 1, 24),
	PerftTest("n1n5/PPPk4/8/8/8/8/4Kppp/5N1N b - - 0 1", 2, 496),
	PerftTest("n1n5/PPPk4/8/8/8/8/4Kppp/5N1N b - - 0 1", 3, 9483),
	PerftTest("n1n5/PPPk4/8/8/8/8/4Kppp/5N1N b - - 0 1", 4, 182838),
	PerftTest("n1n5/PPPk4/8/8/8/8/4Kppp/5N1N b - - 0 1", 5, 3605103),
	PerftTest("n1n5/PPPk4/8/8/8/8/4Kppp/5N1N b - - 0 1", 6, 71179139),
};

int main () {
	bool anyFail = false;
	std::chrono::time_point start = std::chrono::high_resolution_clock::now();
	const unsigned long long allocationsBeforeTests = allocs;
	for (const PerftTest& test : TESTS) {
		unsigned long long res = testing::perft(test.fen, test.depth, false);
		if (test.expected != res) {
			std::cout << "ERROR: Perft test fail: Fen: "
				<< test.fen << ", Depth: " << test.depth << ", Expected: " << test.expected << ", Result: " << res << '\n';
			anyFail = true;
		}
		else {
			std::cout << "Fen: " << test.fen << ", Depth: " << test.depth << ", Expected: " << test.expected << " -> PASS!" << '\n';
		}
	}
	const unsigned long long allocationsAfterTests = allocs;
	std::chrono::duration<float> duration = std::chrono::high_resolution_clock::now() - start;
	std::cout << "Tests Finished!" << '\n';
	if (!anyFail) {
		std::cout << "All tests passed! Great success!" << '\n';
	}
	std::cout << "Time: " << duration.count() << '\n';
	std::cout << "Number of heap allocations: " << allocationsAfterTests - allocationsBeforeTests << '\n';
	std::cout << "Total heap allocations size: " << totalAllocatedMem << " Bytes or "
		<< static_cast<float>((((totalAllocatedMem / 1024.0f) / 1024.0f) / 1024.0f)) << " GB" << '\n';
}
