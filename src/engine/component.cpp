#include "engine/component.h"

#include "engine/engine.h"
#include "engine/entity.h"

const ComponentType EngineComponentType::Camera = EngineComponentType(1);
const ComponentType EngineComponentType::StaticBody = EngineComponentType(10);
const ComponentType EngineComponentType::RigidBody = EngineComponentType(11);
const ComponentType EngineComponentType::RenderRect = EngineComponentType(20);

Component::Component(ComponentType type) : m_Type(type), m_Active(true) {}

void Component::SetActive(bool value) {
	if (m_Active == value) return;

	m_Active = value;

	if (value) {
		OnActivate();
	} else {
		OnDeactivate();
	}
}

void Component::Setup() {}
void Component::Cleanup() {}

void Component::OnActivate() {}
void Component::OnDeactivate() {}

void Component::PreFixedUpdate() {}
void Component::FixedUpdate() {}
void Component::PostFixedUpdate() {}

void Component::PreUpdate() {}
void Component::Update() {}
void Component::PostUpdate() {}

void Component::Render() {}

void Component::OnHit(Hit* hit) {}