#pragma once

#include <memory>
#include <vector>

#include "engine/components/physics.h"

// A container for entities to exist in with various logic for queuing addition
// and deletion during an ongoing update to be handled safely. Also offers
// various handy iterators, eg. caching physics bodies for O(n) iteration. They
// can be easily used as an object pool too (as seen in 'player.h' and
// 'enemy_manager.h')!

// A trick to allow for 'std::make_shared' to work with the private
// constructor is used!
// Source: https://stackoverflow.com/a/20961251

class Entity;
class EntityCollection;

class EntityCollection : public std::enable_shared_from_this<EntityCollection> {
   private:
	EntityCollection();

   public:
	static std::shared_ptr<EntityCollection> Create();
	static void Delete(int id);

	void ProcessAddQueue();
	void ProcessRemoveQueue();

	void Add(std::shared_ptr<Entity>&& entity);

	void Remove(std::shared_ptr<Entity> entity);
	void RemoveAt(int index);

	void Clear();

	std::shared_ptr<Entity>& operator[](const int index);

	std::vector<std::shared_ptr<Entity>>& GetEntities();

	std::vector<std::shared_ptr<Entity>>& GetActiveEntities();
	std::vector<std::shared_ptr<Entity>>& GetInactiveEntities();

	std::vector<std::shared_ptr<StaticBody>>& GetStaticBodies();
	std::vector<std::shared_ptr<RigidBody>>& GetRigidBodies();

	inline int GetId() const { return m_Id; }

   protected:
	friend Engine;
	friend Entity;

	void RegisterStaticBody(std::shared_ptr<StaticBody> staticBody);
	void RegisterRigidBody(std::shared_ptr<RigidBody> rigidBody);

	void UnregisterStaticBody(std::shared_ptr<StaticBody> staticBody);
	void UnregisterRigidBody(std::shared_ptr<RigidBody> rigidBody);

	void SetEntityActive(std::shared_ptr<Entity> entity, bool active);

	void RegisterEntityActive(std::shared_ptr<Entity> entity);
	void RegisterEntityInactive(std::shared_ptr<Entity> entity);
	void UnregisterEntityActive(std::shared_ptr<Entity> entity);
	void UnregisterEntityInactive(std::shared_ptr<Entity> entity);

   private:
	void RemoveInternal(std::shared_ptr<Entity> entity, int index);

	void AddEntity(std::shared_ptr<Entity>&& entity);
	void RemoveEntity(std::shared_ptr<Entity> entity, int index);

   private:
	std::vector<std::shared_ptr<Entity>> m_Entities;

	std::vector<std::shared_ptr<Entity>> m_ActiveEntities;
	std::vector<std::shared_ptr<Entity>> m_InactiveEntities;

	std::vector<std::shared_ptr<StaticBody>> m_StaticBodies;
	std::vector<std::shared_ptr<RigidBody>> m_RigidBodies;

	std::vector<std::shared_ptr<Entity>> m_AddQueue;
	std::vector<std::pair<std::shared_ptr<Entity>, int>> m_RemoveQueue;

	bool m_ClearQueued;
	int m_Id;

	struct MakeSharedEnabler;
};
