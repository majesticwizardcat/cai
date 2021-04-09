#include "ai/neural-network.h"

#include <iostream>
#include <algorithm>

void Neuron::connectLayer(Layer& layer, bool random) {
	for (auto& n : layer.neurons) {
		float weight = random ? RGEN.get(MIN_W, MAX_W) : 0.0f;
		connectNeuron(weight, &n);
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

void Layer::connectLayer(Layer& layer, bool random) {
	for (auto& n : neurons) {
		n.connectLayer(layer, random);
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
	connectForward(0, true);
}

void NeuralNetwork::connectForward(int startIndex, bool random) {
	for (int i = startIndex; i < m_layers.size() - 1; ++i) {
		m_layers[i].connectLayer(m_layers[i + 1], random);
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

void NeuralNetwork::buildFromParents(const NeuralNetwork& n0, const NeuralNetwork& n1,
	float mutationProb) {
	m_layers.clear();
	int layers = std::min(n0.m_layers.size(), n1.m_layers.size());
	m_layers.reserve(layers);
	for (int i = 0; i < layers; ++i) {
		int neurons = std::min(n0.m_layers[i].neurons.size(), n1.m_layers[i].neurons.size());
		m_layers.emplace_back(neurons);
	}
	connectForward(0, false);
	for (int l = 0; l < m_layers.size(); ++l) {
		for (int n = 0; n < m_layers[l].neurons.size(); ++n) {
			for (int c = 0; n < m_layers[l].neurons[n].connections.size(); ++c) {
				m_layers[l].neurons[n].connections[c].weight = RGEN.get() < 0.5f
					? n0.m_layers[l].neurons[n].connections[c].weight
					: n1.m_layers[l].neurons[n].connections[c].weight;
			}
		}
	}
	mutate(mutationProb);
}

void NeuralNetwork::mutate(float mutationProb) {
	if (mutationProb <= 0.0f) {
		return;
	}
	for (auto& l : m_layers) {
		for (auto& n : l.neurons) {
			for (auto& c : n.connections) {
				float p = RGEN.get();
				if (p < mutationProb) {
					p = p / mutationProb;
					c.weight = mutateWeight(c.weight, p);
				}
			}
		}
	}
}

float NeuralNetwork::mutateWeight(float weight, float p) const {
	if (p < MUTATION_TYPE_W) {
		return -weight;
	}
	if (p >= MUTATION_TYPE_W && p < MUTATION_TYPE_W * 2.0f) {
		return (RGEN.get(-1.0f, 1.0f) + weight) * 0.5f;
	}
	return RGEN.get(-1.0f, 1.0f);
}
