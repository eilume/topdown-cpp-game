#include "game/components/bullet.h"

#include "engine/components/physics.h"
#include "engine/engine.h"
#include "engine/entity.h"
#include "game/component.h"

Bullet::Bullet() : Component(GameComponentType::Bullet), vel(Vec2()) {}

void Bullet::Setup() {
	ASSERT(m_Entity->HasComponent(EngineComponentType::RigidBody));
	rb = m_Entity->GetComponent<RigidBody>(EngineComponentType::RigidBody);
}

void Bullet::OnActivate() {
	rb->vel = vel;
	m_Timer = 0.0;
}

void Bullet::FixedUpdate() {
	m_Timer += Engine::Instance()->GetTimeState()->GetFixedStep();

	if (m_Timer >= m_TimerDuration) m_Entity->SetActive(false);
}

void Bullet::OnHit(Hit* hit) {
	if (hit->hitBody->collisionLayer & 0b00000010) {
		// Hit obstacle
		m_Entity->SetActive(false);
	}
}