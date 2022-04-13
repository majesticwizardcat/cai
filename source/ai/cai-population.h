#pragma once

#include <nnpp.hpp>

typedef NNPopulation<float> CAIPopulation;

static const std::vector<uint> PROPAGATOR_LAYOUT = { 64, 100, 100, 80, 64 };
static const std::vector<uint> ANALYZER_LAYOUT = { 64, 100, 80, 80, 30, 10, 1 };
static const std::vector<uint> FAST_LOOKUP_LAYOUT = { 64, 30, 30, 10, 1 };
static const std::vector<uint> CYCLES_MANAGER_LAYOUT = { 3, 3, 1 };

static const uint PROPAGATOR_NETWORK_INDEX = 0;
static const uint ANALYZER_NETWORK_INDEX = 1;
static const uint FAST_LOOKUP_NETWORK_INDEX = 2;
static const uint CYCLES_MANAGER_NETWORK_INDEX = 3;

static const float MIN_AI_WEIGHT_VALUE = -1.0f;
static const float MAX_AI_WEIGHT_VALUE = 1.0f;

static const std::vector<std::vector<uint>> CAI_LAYERS = {
	PROPAGATOR_LAYOUT,
	ANALYZER_LAYOUT,
	FAST_LOOKUP_LAYOUT,
	CYCLES_MANAGER_LAYOUT
};

std::unique_ptr<CAIPopulation> createCAIPopulation(const std::string& name, uint size);
