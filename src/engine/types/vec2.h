#pragma once

#include <cmath>

#include "engine/mathutils.h"

// A 2D vector data type that is used extensively through out the program, with
// various helper functions commonly used in game-play + graphics programming

struct Vec2 {
	float x;
	float y;

	Vec2() : x(0.0f), y(0.0f) {}
	Vec2(float both) : x(both), y(both) {}
	Vec2(float x, float y) : x(x), y(y) {}

	Vec2(const Vec2& source) : x(source.x), y(source.y) {}
	Vec2(Vec2&& source) : x(source.x), y(source.y) {}

	inline Vec2& operator=(const Vec2& source) {
		x = source.x;
		y = source.y;

		return *this;
	}

	inline Vec2& operator=(Vec2&& source) {
		x = source.x;
		y = source.y;

		return *this;
	}

	inline float& operator[](int index) { return index < 1 ? x : y; }

	inline bool IsZero() const { return x == 0 && y == 0; }

	inline bool operator==(Vec2 other) const {
		return x == other.x && y == other.y;
	}
	inline bool operator!=(Vec2 other) const {
		return x != other.x || y != other.y;
	}

	inline Vec2 operator+(Vec2 other) const {
		return Vec2(x + other.x, y + other.y);
	}
	inline Vec2 operator+(float other) const {
		return Vec2(x + other, y + other);
	}

	inline void operator+=(Vec2 other) {
		x += other.x;
		y += other.y;
	}
	inline void operator+=(float other) {
		x += other;
		y += other;
	}

	inline Vec2 operator-(Vec2 other) const {
		return Vec2(x - other.x, y - other.y);
	}
	inline Vec2 operator-(float other) const {
		return Vec2(x - other, y - other);
	}

	inline void operator-=(Vec2 other) {
		x -= other.x;
		y -= other.y;
	}
	inline void operator-=(float other) {
		x -= other;
		y -= other;
	}

	inline Vec2 operator*(Vec2 other) const {
		return Vec2(x * other.x, y * other.y);
	}
	inline Vec2 operator*(float other) const {
		return Vec2(x * other, y * other);
	}

	inline void operator*=(Vec2 other) {
		x *= other.x;
		y *= other.y;
	}
	inline void operator*=(float other) {
		x *= other;
		y *= other;
	}

	inline Vec2 operator/(Vec2 other) const {
		return Vec2(x / other.x, y / other.y);
	}
	inline Vec2 operator/(float other) const {
		return Vec2(x / other, y / other);
	}

	inline void operator/=(Vec2 other) {
		x /= other.x;
		y /= other.y;
	}
	inline void operator/=(float other) {
		x /= other;
		y /= other;
	}

	inline float Magnitude() { return sqrt(x * x + y * y); }

	inline Vec2 Normalized() {
		float mag = Magnitude();
		return mag > 0 ? Vec2(x / mag, y / mag) : Vec2();
	}

	inline Vec2 RotateByRads(float radians) {
		float sin = sinf(radians);
		float cos = cosf(radians);

		return Vec2((cos * x) - (sin * y), (sin * x) + (cos * y));
	}

	inline Vec2 RotateByDeg(float degrees) {
		return RotateByRads(degrees * MathUtils::DegToRad);
	}

	inline float GetAngleRads(Vec2 baseVector = Vec2()) {
		Vec2 baseNorm = baseVector.Normalized();
		Vec2 thisNorm = Normalized();

		return atan2(thisNorm.y - baseNorm.y, thisNorm.x - baseNorm.x);
	}

	inline float GetAngleDeg(Vec2 baseVector = Vec2()) {
		return GetAngleRads() * MathUtils::RadToDeg;
	}

	inline Vec2 Abs() { return (Vec2){fabsf(x), fabsf(y)}; }

	inline static Vec2 Lerp(Vec2 a, Vec2 b, float t) { return a + (b - a) * t; }

	// TODO:
	inline static Vec2 RandomInCircle(float radius) {
		return Vec2(radius, 0).RotateByRads(float(rand()) / float((RAND_MAX)) *
											M_PI * 2);
	}
};
