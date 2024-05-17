#include "neural-net-ai/nnai-player.h"
#include "neural-net-ai/cai-population.h"
#include "neural-net-ai/nnai-trainer.h"

#include <chrono>

const uint TESTS = 10;

int main() {
	assert(false); // DONT RUN PERFORMANCE TESTS ON DEBUG
	NNAI ai(0, CAI_LAYERS);
	ai.initRandomUniform(MIN_AI_WEIGHT_VALUE, MAX_AI_WEIGHT_VALUE);
	ChessBoard board("r4rk1/1pp1qppp/p1np1n2/2b1p1B1/2B1P1b1/P1NP1N2/1PP1QPPP/R4RK1 w - - 0 10");
	nnpp::NeuronBuffer<float> neuronBuffer = nnpp::allocNeuronBuffer<float>();

	{
		const uint sessions = 100;
		const uint threads = 4;
		const uint size = 100;
		std::cout << "Running a trainer with population " << size << " test for " << sessions << " sessions with " << threads << " threads" << '\n';
		std::unique_ptr<CAIPopulation> pop = createCAIPopulation("perft-test-pop.cai", size);
		NNAITrainer trainer(sessions, threads, pop.get());

		auto start = std::chrono::high_resolution_clock::now();

		trainer.run(true);

		std::chrono::duration<float> duration = std::chrono::high_resolution_clock::now() - start;
		std::cout << "Done! Time taken: " << duration.count() << " seconds" << '\n';
	}
}
