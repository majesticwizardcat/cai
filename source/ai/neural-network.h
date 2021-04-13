#pragma once

class Neuron;
class Connection;
class Layer;
class NeuralNetwork;
struct NNSaveData;

#include "tools/random-generator.h"
#include "tools/util.h"

#include <string>
#include <vector>
#include <fstream>

static RandomGenerator RGEN;
const float MIN_W = -1.0f;
const float MAX_W = 1.0f;
const float MUTATION_TYPES = 3.0f;
const float MUTATION_TYPE_W = 1.0f / MUTATION_TYPES;

class Connection {
public:
	float weight;
	Neuron* to;

	Connection() = delete;
	Connection(float weight, Neuron* to) : weight(weight), to(to) { }
	Connection(const Connection& other) = delete;
	Connection(Connection&& other) noexcept : weight(std::move(other.weight)),
		to(std::move(other.to)) { }
	inline bool operator==(const Connection& other) const {
		return util::equals(weight, other.weight);
	}

	inline bool operator!=(const Connection& other) const {
		return !util::equals(weight, other.weight);
	}

};

class Neuron {
public:
	float value;
	std::vector<Connection> connections;

	Neuron() : value(0.0f) { }
	Neuron(float value) : value(value) { }
	Neuron(const Neuron& other) = delete;
	Neuron(Neuron&& other) noexcept : value(std::move(other.value)),
		connections(std::move(other.connections)) { }

	void connectLayer(Layer& layer, bool random);
	void connectNeuron(float weight, Neuron* n);
	void feedForward();
	bool operator==(const Neuron& other) const;
	inline bool operator!=(const Neuron& other) const { return !(*this == other); }
};

class Layer {
public:
	std::vector<Neuron> neurons;
	Layer() = delete;
	Layer(int neurons) : neurons(neurons) { }
	Layer(const Layer& other) = delete;
	Layer(Layer&& other) noexcept : neurons(std::move(other.neurons)) { }

	void connectLayer(Layer& layer, bool random);
	void latch(const std::vector<float> values);
	void feedForward();
	bool operator==(const Layer& other) const;
	bool operator!=(const Layer& other) const { return !(*this == other); }
	inline int size() const { return neurons.size(); }
};

struct NNSaveData {
	std::vector<int> layers;
	std::vector<float> weights;

	NNSaveData() { }
	NNSaveData(const NNSaveData& other)
		: layers(other.layers), weights(other.weights) { }
	NNSaveData(NNSaveData&& other) : layers(std::move(other.layers)),
		weights(std::move(other.weights)) { }
	NNSaveData(const char* location) {
		loadFromDisk(location);
	}

	NNSaveData& operator=(NNSaveData other) {
		layers = std::move(other.layers);
		weights = std::move(other.weights);
		return *this;
	}

	void loadFromDisk(const char* location);
	void writeToDisk(const char* location) const;
	std::string asString() const;
	void loadFromStream(std::ifstream& inputFile);
};

class NeuralNetwork {
private:
	std::vector<Layer> m_layers;

	void connectForward(int startIndex, bool random);
	float mutateWeight(float weight, float p) const;

public:
	NeuralNetwork() = delete;
	NeuralNetwork(const NeuralNetwork& other) = delete;
	NeuralNetwork(NeuralNetwork&& other) noexcept : m_layers(std::move(other.m_layers)) { }
	NeuralNetwork(const std::vector<int> layout);
	NeuralNetwork(const NeuralNetwork& n0, const NeuralNetwork& n1, float mutationProb);
	NeuralNetwork(const char* location);
	NeuralNetwork(const NNSaveData& saveData);

	std::vector<float> feed(const std::vector<float> input);
	void buildFromParents(const NeuralNetwork& n0, const NeuralNetwork& n1, float mutationProb);
	void buildFromLayout(const std::vector<int> layout, bool random);
	void buildFromSave(const NNSaveData& data);
	void mutate(float mutationProb);
	NNSaveData toSaveData() const;
	bool operator==(const NeuralNetwork& other) const;

	inline void save(const char* location) const {
		toSaveData().writeToDisk(location);
	}
};
