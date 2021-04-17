#include "ai/neural-network.h"
#include "ai/ai.h"
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
	ai.save("ai-save.cai");
	std::cout << "Loading AI test: ";
	AI loadedAI("ai-save.cai");
	if (ai == loadedAI) {
		std::cout << "PASS";
	}
	else {
		std::cout << "FAIL";
	}
	std::cout << '\n';
	out = ai.propagate(input, 10000);
	outLoad = loadedAI.propagate(input, 10000);
	std::cout << "Equal ai propagation output: ";
	if (areEqual(out, outLoad, &value, &expected)) {
		std::cout << "PASS" << '\n';
	}
	else {
		std::cout << "FAIL, found: "
			<< value << " expected: " << expected << '\n';
	}
}

