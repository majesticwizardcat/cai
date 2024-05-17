#pragma once

#include <nnpp.hpp>

typedef nnpp::NNPopulation<float> CAIPopulation;

static const std::vector<uint> ANALYZER_LAYOUT = { 64, 72, 95, 190, 250, 380, 444, 612, 900, 1124, 1234, 1013, 900, 712, 520, 400, 290, 220, 180, 150, 90, 64, 55, 44, 33, 23, 13, 11, 7, 5, 3, 1 };

static const uint ANALYZER_NETWORK_INDEX = 0;

static const float MIN_AI_WEIGHT_VALUE = -0.2f;
static const float MAX_AI_WEIGHT_VALUE = 0.2f;

static const std::vector<std::vector<uint>> CAI_LAYERS = {
	ANALYZER_LAYOUT,
};

std::unique_ptr<CAIPopulation> createCAIPopulation(const std::string& name, uint size);
