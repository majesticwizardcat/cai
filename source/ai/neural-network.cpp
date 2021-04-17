#include "ai/neural-network.h"

#include <iostream>
#include <algorithm>
#include <fstream>

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

bool Neuron::operator==(const Neuron& other) const {
	if (connections.size() != other.connections.size()) {
		std::cout << "Neurons have different connections" << '\n';
		return false;
	}
	for (int i = 0; i < connections.size(); ++i) {
		if (connections[i] != other.connections[i]) {
			std::cout << "Different weight" << '\n';
			return false;
		}
	}
	return true;
}

void Layer::connectLayer(Layer& layer, bool random) {
	for (auto& n : neurons) {
		n.connectLayer(layer, random);
	}
}

void Layer::latch(const std::vector<float> values) {
	int i = 0;
	for (auto& n : neurons) {
		n.value = values[i++];
	}
}

void Layer::feedForward() {
	for (auto& n : neurons) {
		n.feedForward();
	}
}

bool Layer::operator==(const Layer& other) const {
	if (neurons.size() != other.neurons.size()) {
		std::cout << "Layers have different sizes" << '\n';
		return false;
	}
	for (int i = 0; i < neurons.size(); ++i) {
		if (neurons[i] != other.neurons[i]) {
			return false;
		}
	}
	return true;
}

void NNSaveData::writeToDisk(const char* location) const {
	std::ofstream outfile(location);
	outfile << asString();
	outfile.close();
}

void NNSaveData::loadFromDisk(const char* location) {
	std::ifstream inFile(location);
	loadFromStream(inFile);
	inFile.close();
}

std::string NNSaveData::asString() const {
	std::string str;
	str += std::to_string(layers.size()) + '\n';
	for (int l : layers) {
		str += std::to_string(l) + '\n';
	}
	for (float w : weights) {
		str += std::to_string(w) + '\n';
	}
	return std::move(str);
}

void NNSaveData::loadFromStream(std::ifstream& inputFile) {
	int layersNum;
	inputFile >> layersNum;
	for (int i = 0; i < layersNum; ++i) {
		int layerSize;
		inputFile >> layerSize;
		layers.push_back(layerSize);
	}
	float w;
	for (int i = 0; i < layers.size() - 1; ++i) {
		for (int j = 0; j < layers[i]; ++j) {
			for (int k = 0; k < layers[i + 1]; ++k) {
				inputFile >> w;
				weights.push_back(w);
			}
		}
	}
}

NeuralNetwork::NeuralNetwork(const std::vector<int> layout) {
	buildFromLayout(layout, true);
}

NeuralNetwork::NeuralNetwork(const NeuralNetwork& n0, const NeuralNetwork& n1, float mutationProb) {
	buildFromParents(n0, n1, mutationProb);
}

NeuralNetwork::NeuralNetwork(const char* location) {
	buildFromSave(NNSaveData(location));
}

NeuralNetwork::NeuralNetwork(const NNSaveData& saveData) {
	buildFromSave(saveData);
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
	m_layers[0].latch(input);

	for (auto& l : m_layers) {
		l.feedForward();
		for (auto& n : l.neurons) {
			n.value = active(n.value);
		}
	}

	std::vector<float> out;
	out.reserve(m_layers.back().size());
	for (const auto& n : m_layers.back().neurons) {
		out.push_back(n.value);
	}
	return std::move(out);
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

void NeuralNetwork::buildFromLayout(const std::vector<int> layout, bool random) {
	if (layout.size() < 2) {
		std::cout << "Error: Attempting to create neural network with less than 2 layrers -> Exiting" << '\n';
		exit(0);
	}
	m_layers.reserve(layout.size());
	for (int l : layout) {
		m_layers.emplace_back(l);
	}
	connectForward(0, random);
}

void NeuralNetwork::buildFromSave(const NNSaveData& data) {
	buildFromLayout(data.layers, false);
	int nextWeight = 0;
	for (auto& l : m_layers) {
		for (auto& n : l.neurons) {
			for (auto& c : n.connections) {
				c.weight = data.weights[nextWeight++];
			}
		}
	}
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
	return RGEN.get(MIN_W, MAX_W);
}

NNSaveData NeuralNetwork::toSaveData() const {
	NNSaveData saveData;
	for (const auto& l : m_layers) {
		saveData.layers.push_back(l.neurons.size());
		for (const auto& n : l.neurons) {
			for (const auto& c : n.connections) {
				saveData.weights.push_back(c.weight);
			}
		}
	}
	return std::move(saveData);
}

bool NeuralNetwork::operator==(const NeuralNetwork& other) const {
	if (m_layers.size() != other.m_layers.size()) {
		std::cout << "Networks have different sizes" << '\n';
		return false;
	}
	for (int i = 0; i < m_layers.size(); ++i) {
		if (m_layers[i] != other.m_layers[i]) {
			return false;
		}
	}
	return true;
}

