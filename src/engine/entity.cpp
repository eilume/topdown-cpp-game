#include "engine/entity.h"

#include <iostream>

#include "engine/component.h"
#include "engine/components/physics.h"
#include "engine/engine.h"
#include "engine/mathutils.h"
#include "engine/types/entity_collection.h"
#include "utils.h"

#define EntityComponentsFunctionCall(FuncName)           \
	for (const auto& componentPair : m_Components) {     \
		if (!componentPair.second->IsActive()) continue; \
		componentPair.second->FuncName();                \
	}

Entity::Entity(Vec2 pos, Vec2 halfSize)
	: aabb({pos, halfSize}),
	  visualAABB({pos, halfSize}),
	  lastPos(pos),
	  m_Active(true),
	  m_State(EntityState::QueuedForCreation) {}

Entity::Entity(AABB aabb)
	: aabb(aabb),
	  visualAABB(aabb),
	  lastPos(aabb.pos),
	  m_Active(true),
	  m_State(EntityState::QueuedForCreation) {}

void Entity::SetActive(bool value) {
	if (m_Active == value) return;

	m_Active = value;
	m_Collection->SetEntityActive(shared_from_this(), value);

	if (value) {
		OnActivate();
	} else {
		OnDeactivate();
	}
}

void Entity::Setup() {
	for (const auto& componentPair : m_Components) {
		const auto& component = componentPair.second;
		if (!component->IsActive() || component->m_IsSetup) continue;
		component->Setup();
		component->m_IsSetup = true;
	}
}

void Entity::Cleanup() { EntityComponentsFunctionCall(Cleanup) }

void Entity::OnActivate() {
	for (auto component : m_SetupQueue) {
		component->Setup();
	}

	m_SetupQueue.clear();

	EntityComponentsFunctionCall(OnActivate)
}

void Entity::OnDeactivate() { EntityComponentsFunctionCall(OnDeactivate) }

void Entity::PreFixedUpdate() { EntityComponentsFunctionCall(PreFixedUpdate) }

void Entity::FixedUpdate() { EntityComponentsFunctionCall(FixedUpdate) }

void Entity::PostFixedUpdate() { EntityComponentsFunctionCall(PostFixedUpdate) }

void Entity::PreUpdate() {
	visualAABB.pos =
		Vec2::Lerp(lastPos, aabb.pos, Engine::Instance()->GetInterpolation());

	EntityComponentsFunctionCall(PreUpdate)
}

void Entity::Update() { EntityComponentsFunctionCall(Update) }

void Entity::PostUpdate() { EntityComponentsFunctionCall(PostUpdate) }

void Entity::Render() { EntityComponentsFunctionCall(Render) }

void Entity::OnHit(Hit* hit) {
	for (const auto& componentPair : m_Components) {
		if (!componentPair.second->IsActive()) continue;
		componentPair.second->OnHit(hit);
	}
}

bool Entity::HasBody() {
	return m_Components.count(EngineComponentType::StaticBody) ||
		   m_Components.count(EngineComponentType::RigidBody);
}

std::shared_ptr<Body> Entity::GetBody() {
	ASSERT(HasBody());
	return std::dynamic_pointer_cast<Body>(
		m_Components.count(EngineComponentType::StaticBody) > 0
			? m_Components[EngineComponentType::StaticBody]
			: m_Components[EngineComponentType::RigidBody]);
}

std::shared_ptr<Component> Entity::AddComponent(
	std::shared_ptr<Component> component) {
	// Ensure there isn't already the same type of component registered
	ASSERT(m_Components.count(component->m_Type) == 0);

	auto type = component->m_Type;
	component->m_Entity = shared_from_this();
	m_Components.emplace(type, component);

	if (IsActive()) {
		component->Setup();
		if (component->IsActive()) component->OnActivate();
	} else {
		m_SetupQueue.push_back(component);
	}

	if (m_Collection != nullptr) {
		if (type == EngineComponentType::StaticBody) {
			m_Collection->RegisterStaticBody(
				std::dynamic_pointer_cast<StaticBody>(component));
		} else if (type == EngineComponentType::RigidBody) {
			m_Collection->RegisterRigidBody(
				std::dynamic_pointer_cast<RigidBody>(component));
		}
	}

	return component;
}

void Entity::RemoveComponent(ComponentType type) {
	ASSERT(m_Components.count(type));

	auto component = m_Components[type];
	component->Cleanup();

	if (m_Collection != nullptr) {
		if (type == EngineComponentType::StaticBody) {
			m_Collection->UnregisterStaticBody(
				std::dynamic_pointer_cast<StaticBody>(component));
		} else if (type == EngineComponentType::RigidBody) {
			m_Collection->UnregisterRigidBody(
				std::dynamic_pointer_cast<RigidBody>(component));
		}
	}

	m_Components.erase(type);
}