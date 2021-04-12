#include "ai/neural-network.h"
#include "tools/util.h"

#include <iostream>

int main() {
	std::vector<int> layout = { 4, 100, 128, 48, 16, 4 };
	NeuralNetwork n(layout);
	n.save("test-ai.cai");
	NeuralNetwork load("test-ai.cai");
	if (n == load) {
		std::cout << "PASS" << '\n';
	}
	else {
		std::cout << "FAIL" << '\n';
	}
	std::vector<float> input = { -0.53f, 0.2f, -0.11f, 0.8f };
	std::vector<float> out = n.feed(input);
	std::vector<float> outLoad = load.feed(input);
	bool eq = true;
	for (int i = 0; i < out.size(); ++i) {
		if (!util::equals(out[i], outLoad[i])) {
			eq = false;
			break;
		}
	}
	if (eq) {
		std::cout << "PASS" << '\n';
	}
	else {
		std::cout << "FAIL" << '\n';
	}
}

