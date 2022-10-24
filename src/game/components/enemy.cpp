#include "game/components/enemy.h"

#include "engine/components/physics.h"
#include "engine/components/renderables.h"
#include "engine/engine.h"
#include "engine/entity.h"
#include "game/component.h"
#include "game/components/game_manager.h"
#include "game/components/player.h"

Enemy::Enemy() : Component(GameComponentType::Enemy) {}

void Enemy::Setup() {
	ASSERT(m_Entity->HasComponent(EngineComponentType::RigidBody));
	rb = m_Entity->GetComponent<RigidBody>(EngineComponentType::RigidBody);

	ASSERT(m_Entity->HasComponent(EngineComponentType::RenderRect));
	renderRect =
		m_Entity->GetComponent<RenderRect>(EngineComponentType::RenderRect);
}

void Enemy::OnActivate() {
	health = m_MaxHealth;
	renderRect->fillColor = Color::SetAlpha(Color::Yellow, 127);
}

void Enemy::FixedUpdate() {
	if (player->GetEntity()->IsActive()) {
		rb->vel =
			(player->GetEntity()->aabb.pos - m_Entity->aabb.pos).Normalized() *
			110;
	} else {
		rb->vel *= 0.96f;
	}
}

void Enemy::OnHit(Hit* hit) {
	if (hit->hitBody->collisionLayer & 0b00000001) {
		// Hit player
		player->DealDamage();

		m_Entity->SetActive(false);
	} else if (hit->hitBody->collisionLayer & 0b00001000) {
		// Hit bullet
		DealDamage();

		hit->hitBody->GetEntity()->SetActive(false);
	}
}

void Enemy::DealDamage() {
	health -= 1;

	if (health == 0) {
		player->gameManager->AddScore();
		m_Entity->SetActive(false);
	} else {
		renderRect->fillColor = Color::SetAlpha(
			Color::Lerp(Color::Red, Color::Yellow, (float)health / m_MaxHealth),
			127);
	}
}