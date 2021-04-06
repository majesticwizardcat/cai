#include "ai/neural-network.h"

#include <iostream>

void Neuron::connectLayer(Layer& layer) {
	for (auto& n : layer.neurons) {
		connectNeuron(RGEN.get(MIN_W, MAX_W), &n);
	}
}

void Neuron::connectNeuron(float weight, Neuron* n) {
	connections.emplace_back(Connection(weight, n));
}

void Neuron::feedForward() {
	for (auto& con : connections) {
		con.to->value += value * con.weight;
	}
}

void Layer::connectLayer(Layer& layer) {
	for (auto& n : neurons) {
		n.connectLayer(layer);
	}
}

void Layer::feedForward() {
	for (auto& n : neurons) {
		n.feedForward();
	}
}

NeuralNetwork::NeuralNetwork(const std::vector<int> architecture) {
	if (architecture.size() < 2) {
		std::cout << "Error: Attempting to create neural network with less than 2 layrers -> Exiting" << '\n';
		exit(0);
	}

	m_layers.reserve(architecture.size());
	for (int l : architecture) {
		m_layers.emplace_back(l);
	}

	for (int i = 0; i < m_layers.size() - 1; ++i) {
		m_layers[i].connectLayer(m_layers[i + 1]);
	}
}

std::vector<float> NeuralNetwork::feed(const std::vector<float> input) {
	if (input.size() != m_layers[0].size()) {
		std::cout << "Error: Feeding network with different size input -> Exiting" << '\n';
		exit(0);
	}
	
	int i = 0;
	for (auto& n : m_layers[0].neurons) {
		n.value = input[i++];
	}

	m_layers[0].feedForward();
	std::vector<float> out;
	out.reserve(m_layers.back().size());
	for (const auto& n : m_layers.back().neurons) {
		out.push_back(n.value);
	}
}
