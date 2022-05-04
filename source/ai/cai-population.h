#pragma once

#include <nnpp.hpp>

typedef NNPopulation<float> CAIPopulation;

static const std::vector<uint> PROPAGATOR_LAYOUT = { 64, 100, 110, 100, 90, 80, 64 };
static const std::vector<uint> ANALYZER_LAYOUT = { 64, 90, 100, 90, 80, 50, 30, 20, 10, 1 };

static const uint PROPAGATOR_NETWORK_INDEX = 0;
static const uint ANALYZER_NETWORK_INDEX = 1;

static const float MIN_AI_WEIGHT_VALUE = 0.0f;
static const float MAX_AI_WEIGHT_VALUE = 1.0f;

static const std::vector<std::vector<uint>> CAI_LAYERS = {
	PROPAGATOR_LAYOUT,
	ANALYZER_LAYOUT,
};

std::unique_ptr<CAIPopulation> createCAIPopulation(const std::string& name, uint size);
