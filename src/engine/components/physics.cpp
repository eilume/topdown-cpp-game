#include "engine/components/physics.h"

#include <SDL.h>

#include "engine/engine.h"
#include "engine/entity.h"
#include "engine/physics.h"

Body::Body(ComponentType type, uint8_t collisionLayer)
	: Component(type), collisionLayer(collisionLayer) {}

StaticBody::StaticBody(uint8_t collisionLayer)
	: Body(EngineComponentType::StaticBody, collisionLayer) {}

RigidBody::RigidBody(uint8_t collisionLayer, uint8_t collisionMask)
	: Body(EngineComponentType::RigidBody, collisionLayer),
	  vel(Vec2()),
	  collisionMask(collisionMask) {}
