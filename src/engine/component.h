#pragma once

#include <memory>

#include "config.h"
#include "utils.h"

class Engine;
struct Entity;
struct AABB;
struct Hit;

// Enum-like struct since you can't inherit from an enum. This allows me to add
// components specific to the game from outside the engine section of the
// codebase.
// Inspired by: https://stackoverflow.com/a/644639

struct ComponentType {
   public:
	operator int() const { return m_Id; }

   protected:
	ComponentType(int id) : m_Id(id) {}

   private:
	int m_Id;
};

class EngineComponentType : public ComponentType {
   private:
	EngineComponentType(int id) : ComponentType(id) {
		// Ensure there is separation between engine and game components
		ASSERT(id < Config::GameComponentIdOffset);
	}

   public:
	static const ComponentType Camera;
	static const ComponentType StaticBody;
	static const ComponentType RigidBody;
	static const ComponentType RenderRect;
};

// Base Component that is added to entities to modify their behavior in various
// ways via callbacks that are either triggered regularly or conditionally

class Component {
   protected:
	Component(ComponentType type);
	// virtual ~Component();

   public:
	inline bool IsActive() const { return m_Active; }
	void SetActive(bool value);

	inline std::shared_ptr<Entity> GetEntity() const { return m_Entity; }

   protected:
	virtual void Setup();
	virtual void Cleanup();

	virtual void OnActivate();
	virtual void OnDeactivate();

	virtual void PreFixedUpdate();
	virtual void FixedUpdate();
	virtual void PostFixedUpdate();

	virtual void PreUpdate();
	virtual void Update();
	virtual void PostUpdate();

	virtual void Render();

	virtual void OnHit(Hit* hit);

   protected:
	friend class Entity;

	ComponentType m_Type;
	bool m_Active;
	bool m_IsSetup;

	std::shared_ptr<Entity> m_Entity;
};