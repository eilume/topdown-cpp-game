#pragma once

#include <SDL.h>

#include <map>
#include <memory>
#include <string>
#include <vector>

#include "engine/component.h"
#include "engine/physics.h"
#include "engine/types/vec2.h"

// Entities are game objects that store core data such as position, physics
// bounding box (AABB), interpolated visual bounding box (AABB) and components.

class Engine;
class EntityCollection;

class Body;

enum class EntityState {
	Normal,
	QueuedForCreation,
	QueuedForDeletion,
};

class Entity : public std::enable_shared_from_this<Entity> {
   public:
	Entity(Vec2 pos, Vec2 halfSize);
	Entity(AABB aabb);

	inline bool IsActive() const { return m_Active; }
	void SetActive(bool value);

	void Setup();
	void Cleanup();

	void OnActivate();
	void OnDeactivate();

	void PreFixedUpdate();
	void FixedUpdate();
	void PostFixedUpdate();

	void PreUpdate();
	void Update();
	void PostUpdate();

	void Render();

	void OnHit(Hit* hit);

	bool HasBody();
	std::shared_ptr<Body> GetBody();

	inline EntityState GetState() const { return m_State; }
	inline bool CanBeUsed() const {
		return m_State == EntityState::Normal && m_Active;
	}

	std::shared_ptr<Component> AddComponent(
		std::shared_ptr<Component> component);
	void RemoveComponent(ComponentType type);

	inline std::map<ComponentType, std::shared_ptr<Component>>&
	GetComponents() {
		return m_Components;
	}

	bool HasComponent(ComponentType type) const {
		return m_Components.count(type) > 0;
	}

	template <typename T>
	std::shared_ptr<T> GetComponent(ComponentType type) {
		return std::dynamic_pointer_cast<T>(m_Components[type]);
	}

   private:
	inline void SetState(EntityState state) { m_State = state; }

   public:
	AABB aabb;
	AABB visualAABB;
	Vec2 lastPos;

	std::string name;

   private:
	friend class EntityCollection;
	friend class Physics;

	EntityState m_State;
	bool m_Active;

	std::map<ComponentType, std::shared_ptr<Component>> m_Components;
	std::shared_ptr<EntityCollection> m_Collection;

	std::vector<std::shared_ptr<Component>> m_SetupQueue;
};
