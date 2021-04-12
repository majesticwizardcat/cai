#include "ai/ai.h"

#include <utility>

AI::AI() : m_propagator(PROPAGATOR_LAYOUT), m_analyzer(ANALYZER_LAYOUT),
	m_fastLookup(FAST_LOOKUP_LAYOUT), m_cyclesManager(CYCLES_MANAGER_LAYOUT),
	m_score(0.0f) { }

AI::AI(AI&& other) : m_propagator(std::move(other.m_propagator)),
	m_analyzer(std::move(other.m_analyzer)),
	m_fastLookup(std::move(other.m_fastLookup)),
	m_cyclesManager(std::move(other.m_cyclesManager)),
	m_score(std::move(other.m_score)) { }

