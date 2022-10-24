#pragma once

#include "engine/component.h"
#include "engine/types/vec2.h"

class RigidBody;
class RenderRect;
class Player;

class Enemy : public Component {
   public:
	Enemy();

	void Setup() override;

	void OnActivate() override;

	void FixedUpdate() override;

	void OnHit(Hit* hit) override;

	void DealDamage();

   public:
	std::shared_ptr<RigidBody> rb;
	std::shared_ptr<RenderRect> renderRect;

	std::shared_ptr<Player> player;

	int health;

   private:
	const int m_MaxHealth = 3;
};