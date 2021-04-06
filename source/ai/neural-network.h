#pragma once

class Neuron;
class Connection;
class Layer;
class NeuralNetwork;

#include "tools/random-generator.h"

#include <vector>

static RandomGenerator RGEN;
const float MIN_W = -1.0f;
const float MAX_W = 1.0f;

class Connection {
public:
	float weight;
	Neuron* to;

	Connection() = delete;
	Connection(float weight, Neuron* to) : weight(weight), to(to) { }
	Connection(const Connection& other) = delete;
	Connection(Connection&& other) noexcept : weight(std::move(other.weight)),
		to(std::move(other.to)) { }
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

	void connectLayer(Layer& layer);
	void connectNeuron(float weight, Neuron* n);
	void feedForward();
};

class Layer {
public:
	std::vector<Neuron> neurons;
	Layer() = delete;
	Layer(int neurons) : neurons(neurons) { }
	Layer(const Layer& other) = delete;
	Layer(Layer&& other) noexcept : neurons(std::move(other.neurons)) { }

	void connectLayer(Layer& layer);
	void feedForward();
	int size() const { return neurons.size(); }
};

class NeuralNetwork {
private:
	std::vector<Layer> m_layers;

public:
	NeuralNetwork() = delete;
	NeuralNetwork(const NeuralNetwork& other) = delete;
	NeuralNetwork(NeuralNetwork&& other) noexcept : m_layers(std::move(other.m_layers)) { }
	NeuralNetwork(const std::vector<int> architecture);

	std::vector<float> feed(const std::vector<float> input);
};
