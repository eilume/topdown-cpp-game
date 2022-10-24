#pragma once

#include "engine/component.h"
#include "engine/types/vec2.h"

// A simple camera system that is used by renderables to offset and cull render
// operations.

class Camera;

class Camera : public Component, public std::enable_shared_from_this<Camera> {
   public:
	Camera();

   protected:
	void Setup() override;
};