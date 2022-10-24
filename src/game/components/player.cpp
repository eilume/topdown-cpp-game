#include "game/components/player.h"

#include <SDL.h>

#include <algorithm>
#include <iostream>

#include "config.h"
#include "engine/components/camera.h"
#include "engine/components/physics.h"
#include "engine/components/renderables.h"
#include "engine/engine.h"
#include "engine/entity.h"
#include "engine/types/entity_collection.h"
#include "game/component.h"
#include "game/components/bullet.h"
#include "game/components/game_manager.h"

Player::Player() : Component(GameComponentType::Player) {}

void Player::Setup() {
	m_Entity->aabb.halfSize = Vec2(m_PlayerSize);
	m_Entity->visualAABB.halfSize = Vec2(m_PlayerSize);

	ASSERT(m_Entity->HasComponent(EngineComponentType::RigidBody));
	rb = m_Entity->GetComponent<RigidBody>(EngineComponentType::RigidBody);

	ASSERT(m_Entity->HasComponent(EngineComponentType::RenderRect));
	renderRect =
		m_Entity->GetComponent<RenderRect>(EngineComponentType::RenderRect);

	renderRect->renderMode = RenderMode::Both;
	renderRect->fillColor = Color::SetAlpha(Color::VividGreen, 191);
	renderRect->outlineColor = Color::Lime;

	camera = Engine::Instance()->GetCamera();
	camera->GetEntity()->aabb.pos = GetEntity()->aabb.pos;

	size_t gameManagerIndex =
		Engine::Instance()->GetAllActiveEntities().FindIndex(
			[](const std::shared_ptr<Entity>& e) {
				return e->name.compare(std::string("GameManager")) == 0;
			});
	ASSERT(gameManagerIndex != -1);
	gameManager =
		Engine::Instance()
			->GetAllActiveEntities()[gameManagerIndex]
			->GetComponent<GameManager>(GameComponentType::GameManager);

	// Setup bullet object pool
	bullets = EntityCollection::Create();
	for (int i = 0; i < m_BulletPoolSize; ++i) {
		auto bullet = std::make_shared<Entity>((Vec2){0, 0}, (Vec2){6, 6});
		bullet->SetActive(false);

		bullet->AddComponent(std::make_shared<RigidBody>(0b00001000));
		bullet->AddComponent(std::make_shared<RenderRect>(
			RenderMode::Both, Color::SetAlpha(Color::Violet, 127),
			Color::Violet));
		bullet->AddComponent(std::make_shared<Bullet>());

		bullets->Add(std::move(bullet));
	}

	health = m_MaxHealth;

	std::cout << "[Player]: Health Status: " << health << '/' << m_MaxHealth
			  << std::endl;
}

void Player::PreFixedUpdate() {
	m_BulletTimer += Engine::Instance()->GetTimeState()->GetFixedStep();
	m_InvincibilityTimer += Engine::Instance()->GetTimeState()->GetFixedStep();

	if (bullets->GetInactiveEntities().size() > 0 && IsFiring() &&
		m_BulletTimer >= m_BulletFireRate) {
		// Spawn bullet
		auto& bulletEntity = bullets->GetInactiveEntities()[0];
		auto bullet =
			bulletEntity->GetComponent<Bullet>(GameComponentType::Bullet);

		bulletEntity->aabb.pos = m_Entity->aabb.pos;
		bullet->vel = fireDir * 400;

		bulletEntity->SetActive(true);

		m_BulletTimer -= m_BulletFireRate;
	}

	m_BulletTimer = std::min(m_BulletTimer, m_BulletFireRate);
	m_InvincibilityTimer =
		std::min(m_InvincibilityTimer, m_InvincibilityDuration);
}

void Player::FixedUpdate() { rb->vel = moveDir * (IsFiring() ? 90 : 180); }

void Player::PostFixedUpdate() {
	// Make camera follow player
	camera->GetEntity()->aabb.pos =
		Vec2::Lerp(camera->GetEntity()->aabb.pos, GetEntity()->aabb.pos,
				   Engine::Instance()->GetTimeState()->GetFixedStep() * 2.5);
}

void Player::Update() {
	moveDir = Vec2();
	if (Engine::Instance()->GetInput()->GetKey(SDL_SCANCODE_W)) moveDir.y += -1;
	if (Engine::Instance()->GetInput()->GetKey(SDL_SCANCODE_S)) moveDir.y += 1;
	if (Engine::Instance()->GetInput()->GetKey(SDL_SCANCODE_A)) moveDir.x += -1;
	if (Engine::Instance()->GetInput()->GetKey(SDL_SCANCODE_D)) moveDir.x += 1;

	if (IsMoving()) moveDir = moveDir.Normalized();

	fireDir = Vec2();
	if (Engine::Instance()->GetInput()->GetKey(SDL_SCANCODE_UP))
		fireDir.y += -1;
	if (Engine::Instance()->GetInput()->GetKey(SDL_SCANCODE_DOWN))
		fireDir.y += 1;
	if (Engine::Instance()->GetInput()->GetKey(SDL_SCANCODE_LEFT))
		fireDir.x += -1;
	if (Engine::Instance()->GetInput()->GetKey(SDL_SCANCODE_RIGHT))
		fireDir.x += 1;

	if (IsFiring()) {
		fireDir = fireDir.Normalized();
		renderRect->outlineColor = Color::VividPink;
	} else {
		renderRect->outlineColor = Color::Lime;
	}
}

void Player::DealDamage() {
	if (m_InvincibilityTimer >= m_InvincibilityDuration) {
		health -= 1;

		if (health == 0) {
			// Game Over!
			renderRect->renderMode = RenderMode::None;
			rb->SetActive(false);
			GetEntity()->SetActive(false);

			gameManager->GameOver();
		} else {
			renderRect->fillColor = Color::SetAlpha(
				Color::VividGreen,
				(uint8_t)(((float)health / m_MaxHealth) * 127) + 64);

			std::cout << "[Player]: Took 1 damage!" << std::endl;
			std::cout << "          Health Status: " << health << '/'
					  << m_MaxHealth << std::endl;
			std::cout << "          You are now invulnerable for "
					  << m_InvincibilityDuration << " seconds!" << std::endl;
		}

		gameManager->damageFlashRect->fillColor.a = 63;

		m_InvincibilityTimer -= m_InvincibilityDuration;
	}
}

bool Player::IsMoving() { return moveDir.Magnitude() > 0.01; }

bool Player::IsFiring() { return fireDir.Magnitude() > 0.01; }