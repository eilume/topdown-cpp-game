#pragma once

#include "engine/component.h"
#include "engine/types/vec2.h"

// Bullets spawned by the player that collide and despawn enemies

class RigidBody;

class Bullet : public Component {
   public:
	Bullet();

	void Setup() override;

	void OnActivate() override;

	void FixedUpdate() override;

	void OnHit(Hit* hit) override;

   public:
	std::shared_ptr<RigidBody> rb;

	Vec2 vel;

   private:
	const double m_TimerDuration = 3.0;
	double m_Timer;
};