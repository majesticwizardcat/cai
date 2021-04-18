#include "ai/neural-network.h"
#include "ai/ai.h"
#include "ai/ai-population.h"
#include "tools/util.h"
#include "tools/random-generator.h"

#include <iostream>

const std::vector<int> TEST_LAYOUT = { 64, 100, 156, 80, 64 };
static RandomGenerator rgen;

bool areEqual(const std::vector<float> out0, const std::vector<float>& out1, float* value, float* expected) {
	for (int i = 0; i < out0.size(); ++i) {
		if (std::isinf(out0[i]) || std::isnan(out0[i]) || !util::equals(out0[i], out1[i])) {
			*value = out0[i];
			*expected = out1[i];
			return false;
		}
	}
	return true;
}

bool testAI(AI& ai, bool verbose) {
	float value, expected;
	std::vector<float> out;
	std::vector<float> outLoad;
	std::vector<float> input;
	for (int i = 0; i < 64; ++i) {
		input.push_back(rgen.get(-1.0f, 1.0f));
	}
	ai.save("ai-save.cai");
	if (verbose) {
		std::cout << "Loading AI test: ";
	}
	AI loadedAI("ai-save.cai");
	if (ai == loadedAI) {
		if (verbose) {
			std::cout << "PASS";
		}
	}
	else {
		if (verbose) {
			std::cout << "FAIL";
		}
		return false;
	}
	if (verbose) {
		std::cout << '\n';
		std::cout << "Equal ai analyzer output: ";
	}
	float analysis = ai.analyze(input, 10000);
	float analysisLoad = loadedAI.analyze(input, 10000);
	if (util::equals(analysis, analysisLoad)) {
		if (verbose) {
			std::cout << "PASS" << '\n';
		}
	}
	else {
		if (verbose) {
			std::cout << "FAIL, found: "
				<< analysis << " expected: " << analysisLoad << '\n';
		}
		return false;
	}
	if (verbose) {
		std::cout << "Equal ai lookup output: ";
	}
	analysis = ai.fastLookup(input);
	analysisLoad = loadedAI.fastLookup(input);
	if (util::equals(analysis, analysisLoad)) {
		if (verbose) {
			std::cout << "PASS" << '\n';
		}
	}
	else {
		if (verbose) {
			std::cout << "FAIL, found: "
				<< analysis << " expected: " << analysisLoad << '\n';
		}
		return false;
	}
	if (verbose) {
		std::cout << "Equal ai cycles output: ";
	}
	float cycles = ai.cycles(12345, 13);
	float cyclesload = loadedAI.cycles(12345, 13);
	if (util::equals(cycles, cyclesload)) {
		if (verbose) {
			std::cout << "PASS" << '\n';
		}
	}
	else {
		if (verbose) {
			std::cout << "FAIL, found: "
				<< cycles << " expected: " << cyclesload << '\n';
		}
		return false;
	}
	return true;
}

void testPopulation(AIPopulation& population) {
	std::cout << "Repopulating...";
	for (int i = 0; i < 123; ++i) {
		population.repopulate();
	}
	std::cout << "DONE" << '\n';
	population.save("population.cai");
	AIPopulation loaded("population.cai");
	std::cout << "Loading population test: ";
	if (population == loaded) {
		std::cout << "PASS" << '\n';
	}
	else {
		std::cout << "FAIL" << '\n';
	}
	std::cout << "Checking populations: ";
	bool pass = true;
	for (int i = 0; i < population.getPopulationSize(); ++i) {
		if (!testAI(*(population.getAI(i)), false)) {
			pass = false;
			break;
		}
	}
	if (pass) {
		std::cout << "PASS";
	}
	else {
		std::cout << "FAIL";
	}
	std::cout << '\n';

	std::cout << "Repopulating...";
	for (int i = 0; i < 123; ++i) {
		population.repopulate();
	}
	std::cout << "DONE" << '\n';
	std::cout << "Testing repopulation: ";
	population.save("population0.cai");
	loaded.loadFromDisk("population0.cai");
	if (population == loaded) {
		std::cout << "PASS" << '\n';
	}
	else {
		std::cout << "FAIL" << '\n';
	}
}

int main() {
	float value, expected;
	NeuralNetwork n(TEST_LAYOUT);
	n.save("test-ai.cai");
	std::cout << "Loading test: ";
	NeuralNetwork load("test-ai.cai");
	if (n == load) {
		std::cout << "PASS";
	}
	else {
		std::cout << "FAIL";
	}
	std::cout << '\n';
	std::vector<float> input;
	for (int i = 0; i < 64; ++i) {
		input.push_back(rgen.get(-1.0f, 1.0f));
	}
	std::vector<float> out = n.feed(input);
	std::vector<float> outLoad = load.feed(input);
	std::cout << "Equal output test: ";
	if (areEqual(out, outLoad, &value, &expected)) {
		std::cout << "PASS" << '\n';
	}
	else {
		std::cout << "FAIL, found: "
			<< value << " expected: " << expected << '\n';
	}
	
	AI ai;
	testAI(ai, true);

	AIPopulation aiPopulation;
	testPopulation(aiPopulation);
}

