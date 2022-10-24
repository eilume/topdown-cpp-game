#pragma once

#include <memory>

// Handles time within the gameloop, schedules how many fixed updates occur in a
// single variable update frame and delta time for interpolating between frames
// easily.

class TimeState {
   public:
	TimeState(uint64_t startPerfCounter, uint64_t perfFreq, double fixedStep);

	int Update(uint64_t perfCounter);

	inline double GetTime() const { return m_Time; }
	inline double GetDeltaTime() const { return m_DeltaTime; }

	inline double GetFixedTime() const { return m_FixedTime; }
	inline double GetFixedStep() const { return m_FixedStep; }
	inline double GetFixedStepProgress() const { return m_FixedStepProgress; }

	inline bool FixedUpdateThisFrame() const { return m_FixedTicks > 0; }

   private:
	uint64_t m_PerfCounter;
	uint64_t m_FixedPerfCounter;

	uint64_t m_PerfAccumulator;
	uint64_t m_StartPerfCounter;
	uint64_t m_PerfFreq;

	double m_Time;
	double m_DeltaTime;

	double m_FixedTime;
	double m_FixedStep;
	double m_FixedStepProgress;

	int m_FixedTicks;
};
