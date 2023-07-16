#include "neural-net-ai/cai-population.h"

std::unique_ptr<CAIPopulation> createCAIPopulation(const std::string& name, uint size) {
	auto pop = std::make_unique<CAIPopulation>(name, size, CAI_LAYERS);
	pop->createRandom(MIN_AI_WEIGHT_VALUE, MAX_AI_WEIGHT_VALUE);
	return pop;
}
