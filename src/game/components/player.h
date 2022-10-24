#pragma once

#include "engine/component.h"
#include "engine/types/vec2.h"

class RigidBody;
class RenderRect;
class Camera;
class GameManager;
class EntityCollection;

class Player : public Component {
   public:
	Player();

	void Setup() override;

	void PreFixedUpdate() override;
	void FixedUpdate() override;
	void PostFixedUpdate() override;

	void Update() override;

	void DealDamage();

	bool IsMoving();
	bool IsFiring();

   public:
	std::shared_ptr<RigidBody> rb;
	std::shared_ptr<RenderRect> renderRect;

	std::shared_ptr<Camera> camera;
	std::shared_ptr<GameManager> gameManager;

	std::shared_ptr<EntityCollection> bullets;

	Vec2 moveDir;
	Vec2 fireDir;

	int health;

   private:
	const int m_BulletPoolSize = 100;
	const int m_MaxHealth = 5;

	const int m_PlayerSize = 16;

	const double m_InvincibilityDuration = 0.5;
	const double m_BulletFireRate = 0.1;

	double m_BulletTimer;
	double m_InvincibilityTimer;
};