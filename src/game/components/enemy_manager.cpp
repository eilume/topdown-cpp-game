#include "game/components/enemy_manager.h"

#include <algorithm>

#include "engine/components/camera.h"
#include "engine/components/physics.h"
#include "engine/components/renderables.h"
#include "engine/engine.h"
#include "engine/entity.h"
#include "engine/types/entity_collection.h"
#include "game/component.h"
#include "game/components/enemy.h"
#include "game/components/player.h"

EnemyManager::EnemyManager() : Component(GameComponentType::EnemyManager) {}

void EnemyManager::Setup() {
	size_t playerIndex = Engine::Instance()->GetAllActiveEntities().FindIndex(
		[](const std::shared_ptr<Entity>& e) {
			return e->name.compare(std::string("Player")) == 0;
		});
	ASSERT(playerIndex != -1);
	player = Engine::Instance()
				 ->GetAllActiveEntities()[playerIndex]
				 ->GetComponent<Player>(GameComponentType::Player);

	enemies = EntityCollection::Create();
	for (int i = 0; i < m_EnemyPoolSize; ++i) {
		auto enemy = std::make_shared<Entity>((Vec2){0, 0}, (Vec2){16, 16});
		enemy->SetActive(false);

		enemy->AddComponent(
			std::make_shared<RigidBody>(0b00000100, 0b00001111));
		enemy->AddComponent(std::make_shared<RenderRect>(
			RenderMode::Both, Color::SetAlpha(Color::Yellow, 127), Color::Red));
		enemy->AddComponent(std::make_shared<Enemy>());

		enemy->GetComponent<Enemy>(GameComponentType::Enemy)->player = player;

		enemies->Add(std::move(enemy));
	}

	camera = Engine::Instance()->GetCamera();

	m_SpawnRadius = camera->GetEntity()->aabb.halfSize.Magnitude() + 8;

	m_CurrentSpawnDelay = m_StartSpawnDelay;
}

void EnemyManager::PreFixedUpdate() {
	m_SpawnTimer += Engine::Instance()->GetTimeState()->GetFixedStep();

	if (enemies->GetInactiveEntities().size() > 0 &&
		m_SpawnTimer >= m_CurrentSpawnDelay) {
		// Spawn enemy
		auto& enemyEntity = enemies->GetInactiveEntities()[0];
		auto enemy = enemyEntity->GetComponent<Enemy>(GameComponentType::Enemy);

		// Keep generating a new position until it's not intersecting all static
		// bodies
		AABB newAABB = enemy->GetEntity()->aabb;
		while (true) {
			newAABB.pos = camera->GetEntity()->aabb.pos +
						  Vec2::RandomInCircle(m_SpawnRadius);

			bool isIntersecting = false;
			for (size_t i = 0;
				 i < Engine::Instance()->GetAllStaticBodies().size(); ++i) {
				auto& staticBody = Engine::Instance()->GetAllStaticBodies()[i];

				if (AABB::CheckIntersection(&staticBody->GetEntity()->aabb,
											&newAABB)) {
					isIntersecting = true;
					break;
				}
			}

			if (!isIntersecting) break;
		}

		enemyEntity->aabb = newAABB;
		enemyEntity->SetActive(true);

		m_SpawnTimer -= m_CurrentSpawnDelay;

		// Decrease spawn delay, effectively causing more enemies to spawn
		// per second, capped to a minimum spawn delay
		m_CurrentSpawnDelay =
			std::max(m_CurrentSpawnDelay - m_SpawnDelayDecreasePerSpawn,
					 m_MinSpawnDelay);
	}
	m_SpawnTimer = std::min(m_SpawnTimer, m_CurrentSpawnDelay);
}