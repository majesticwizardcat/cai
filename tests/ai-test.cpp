#include "ai/neural-network.h"
#include "ai/ai.h"
#include "tools/util.h"

#include <iostream>

int main() {
	float value, expected;
	std::vector<int> layout = { 4, 100, 128, 48, 16, 4 };
	NeuralNetwork n(layout);
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
	std::vector<float> input = { -0.53f, 0.2f, -0.11f, 0.8f };
	std::vector<float> out = n.feed(input);
	std::vector<float> outLoad = load.feed(input);
	bool eq = true;
	std::cout << "Equal output test: ";
	for (int i = 0; i < out.size(); ++i) {
		if (!util::equals(out[i], outLoad[i])) {
			value = out[i];
			expected = outLoad[i];
			eq = false;
			break;
		}
	}
	if (eq) {
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
}

