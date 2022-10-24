#pragma once

#include "engine/component.h"
#include "engine/types/vec2.h"

// Components for the physics system. Static bodies don't have a velocity,
// whilst rigidbodies do.

class Engine;

class Body : public Component {
   protected:
	Body(ComponentType type, uint8_t collisionLayer = 0b00000001);

   public:
	uint8_t collisionLayer;
};

class StaticBody : public Body {
   public:
	StaticBody(uint8_t collisionLayer = 0b00000001);
};

class RigidBody : public Body {
   public:
	RigidBody(uint8_t collisionLayer = 0b00000001,
			  uint8_t collisionMask = 0b11111111);

   public:
	Vec2 vel;
	uint8_t collisionMask;
};
