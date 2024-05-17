#pragma once
#pragma warning(disable:4146)

#include <random>
#include <chrono>

#include "pcg_random.hpp"
#include "pcg_extras.hpp"

class RandomGenerator {
private:
	pcg32_fast m_randomDevice;
	std::uniform_real_distribution<float> m_distribution{ std::uniform_real_distribution<float>(0.0f, 0.9999f) };

public:
	RandomGenerator() :
		m_randomDevice(pcg_extras::seed_seq_from<std::random_device>()) { }

	RandomGenerator(unsigned long long seed) : m_randomDevice(seed) { }

	inline int16_t getInt16() {
		return static_cast<int16_t>(m_randomDevice());
	}

	inline uint32_t getUint32() {
		return m_randomDevice();
	}

	inline float get() {
		return m_distribution(m_randomDevice);
	}

	inline float get(float from, float to) {
		return from + get() * (to - from);
	}
};

