#pragma once

#include <memory>
#include <vector>

#include "engine/component.h"
#include "engine/types/vec2.h"

class Player;
class Camera;
class Enemy;
class EntityCollection;

class EnemyManager : public Component {
   public:
	EnemyManager();

	void Setup() override;

	void PreFixedUpdate() override;

   public:
	std::shared_ptr<Player> player;
	std::shared_ptr<Camera> camera;

	std::shared_ptr<EntityCollection> enemies;

   private:
	const int m_EnemyPoolSize = 50;

	const double m_StartSpawnDelay = 1.1;
	const double m_SpawnDelayDecreasePerSpawn = 0.01;
	const double m_MinSpawnDelay = 0.1;

	double m_SpawnTimer;
	double m_CurrentSpawnDelay;

	float m_SpawnRadius;
};