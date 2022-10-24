#pragma once

#include <SDL.h>

#include <string_view>

#include "engine/color.h"

using namespace ::std::literals::string_view_literals;

namespace Config {
constexpr inline auto GameName = "Topdown Game"sv;

constexpr int ScreenWidth = 720;
constexpr int ScreenHeight = 720;

// TODO: add camera scale instead of doing this
constexpr int UnitSize = 50;

constexpr double FixedTimeStep = 1.0 / 60.0;
constexpr int PhysicsIterations = 4;

constexpr int GameComponentIdOffset = 100;
};	// namespace Config
