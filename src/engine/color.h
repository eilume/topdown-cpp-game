#pragma once

#include <SDL.h>

#include "engine/mathutils.h"

// Collection of predefined colors + helper functions

namespace Color {
// clang-format off
constexpr SDL_Color White      = {255, 255, 255, 255};
constexpr SDL_Color Black      = {  0,   0,   0, 255};

constexpr SDL_Color DarkGray  = { 28,  28,  28, 255};
constexpr SDL_Color Gray       = { 96,  96,  96, 255};
constexpr SDL_Color LightGray = {179, 179, 179, 255};

constexpr SDL_Color Red        = {255,   0,   0, 255};
constexpr SDL_Color Green      = {  0, 255,   0, 255};
constexpr SDL_Color Blue       = {  0,   0, 255, 255};

constexpr SDL_Color Orange     = {255, 127,   0, 255};
constexpr SDL_Color Yellow     = {255, 255,   0, 255};
constexpr SDL_Color Lime       = {127, 255,   0, 255};
constexpr SDL_Color VividGreen = {  0, 255, 127, 255};
constexpr SDL_Color Aqua       = {  0, 255, 255, 255};
constexpr SDL_Color Azure      = {  0, 127, 255, 255};
constexpr SDL_Color Violet     = {127,   0, 255, 255};
constexpr SDL_Color Magenta    = {255,   0, 255, 255};
constexpr SDL_Color VividPink  = {255,   0, 127, 255};
// clang-format on

inline static SDL_Color SetAlpha(SDL_Color color, uint8_t alpha) {
	return {color.r, color.g, color.b, alpha};
}

inline static SDL_Color Lerp(SDL_Color a, SDL_Color b, float t) {
	return {(uint8_t)MathUtils::Lerp(a.r, b.r, t),
			(uint8_t)MathUtils::Lerp(a.g, b.g, t),
			(uint8_t)MathUtils::Lerp(a.b, b.b, t),
			(uint8_t)MathUtils::Lerp(a.a, b.a, t)};
}
}  // namespace Color