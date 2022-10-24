#pragma once

#include <cmath>

// Various helper functions related to maths

class MathUtils {
   public:
	inline static float Lerp(float a, float b, float t) {
		return a + (b - a) * t;
	}

	inline static float CustomRange(float zeroToOne, float min, float max) {
		return (zeroToOne * (max - min)) + min;
	}

	inline static int RandomInt(int min, int max) {
		return (rand() % (max - min)) + min;
	}

	static constexpr double RadToDeg = 180 / M_PI;
	static constexpr double DegToRad = M_PI / 180;
};