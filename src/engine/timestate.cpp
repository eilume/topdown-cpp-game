#include "engine/timestate.h"

#include <algorithm>
#include <cmath>
#include <iostream>

#include "engine/mathutils.h"

// Really useful resource on ways to approach timing for your
// game (engine) + things to keep in mind for specific scenarios:
// https://medium.com/@tglaiel/how-to-make-your-game-run-at-60fps-24c61210fe75

TimeState::TimeState(uint64_t startPerfCounter, uint64_t perfFreq,
					 double fixedStep)
	: m_PerfCounter(0),
	  m_FixedPerfCounter(0),
	  m_PerfAccumulator(0),
	  m_StartPerfCounter(startPerfCounter),
	  m_PerfFreq(perfFreq),
	  m_Time(0.0),
	  m_DeltaTime(0.0),
	  m_FixedTime(0.0),
	  m_FixedStep(fixedStep),
	  m_FixedStepProgress(0.0) {}

int TimeState::Update(uint64_t perfCounter) {
	perfCounter -= m_StartPerfCounter;

	uint64_t deltaCounter = perfCounter - m_PerfCounter;
	uint64_t fixedStepAsPerfCounter = m_FixedStep * m_PerfFreq;

	m_FixedTicks = 0;
	m_PerfAccumulator += deltaCounter;
	while (m_PerfAccumulator >= fixedStepAsPerfCounter) {
		m_FixedTicks++;
		m_PerfAccumulator -= fixedStepAsPerfCounter;
	}

	m_FixedStepProgress = std::clamp(
		((perfCounter - m_FixedPerfCounter) % fixedStepAsPerfCounter) /
			(double)fixedStepAsPerfCounter,
		0.0, 1.0);

	m_PerfCounter = perfCounter;
	m_FixedPerfCounter += m_FixedTicks * fixedStepAsPerfCounter;

	double delta = deltaCounter / (double)m_PerfFreq;

	m_Time += delta;
	m_DeltaTime = delta;
	m_FixedTime += m_FixedTicks * m_FixedStep;

	return m_FixedTicks;
}