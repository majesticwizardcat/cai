#include "ai/cai-population.h"

std::unique_ptr<CAIPopulation> createCAIPopulation(const std::string& name, uint size) {
	return std::make_unique<CAIPopulation>(name, size, CAI_LAYERS, MIN_AI_WEIGHT_VALUE, MAX_AI_WEIGHT_VALUE);
}
