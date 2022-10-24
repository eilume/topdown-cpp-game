#include "engine/types/entity_collection.h"

#include <iostream>

#include "engine/engine.h"
#include "engine/entity.h"

EntityCollection::EntityCollection() {}

void EntityCollection::ProcessAddQueue() {
	if (m_AddQueue.size() > 0) {
		for (auto&& entity : m_AddQueue) {
			AddEntity(std::move(entity));
		}

		m_AddQueue.clear();
	}
}

void EntityCollection::ProcessRemoveQueue() {
	if (m_RemoveQueue.size() > 0) {
		if (m_Entities.size() > m_RemoveQueue.size() && !m_ClearQueued) {
			// Traverse in reverse to not invalidate proceeding indexes from the
			// queue by offsetting the indexes of queued for deletion entities
			for (int i = m_AddQueue.size() - 1; i >= 0; i--) {
				auto& pair = m_RemoveQueue[i];
				RemoveEntity(pair.first, pair.second);
			}
		} else {
			m_Entities.clear();
			m_ClearQueued = false;
		}

		m_RemoveQueue.clear();
	}
}

void EntityCollection::Add(std::shared_ptr<Entity>&& entity) {
	entity->m_Collection = shared_from_this();
	if (Engine::Instance()->CanAddOrRemoveEntities()) {
		// Can safely add the entity instantly
		AddEntity(std::move(entity));
	} else {
		// Must queue entity to be added later
		entity->SetState(EntityState::QueuedForCreation);
		m_AddQueue.emplace_back(std::move(entity));
	}
}

void EntityCollection::Remove(std::shared_ptr<Entity> entity) {
	auto it = std::find(m_Entities.begin(), m_Entities.end(), entity);

	ASSERT(it != m_Entities.end());
	int index = std::distance(m_Entities.begin(), it);

	RemoveInternal(entity, index);
}

void EntityCollection::RemoveAt(int index) {
	if (index < 0 || index >= m_Entities.size()) {
		std::cerr << "Can't remove entity at index '" << index << "'"
				  << std::endl;
		return;
	}

	RemoveInternal(m_Entities[index], index);
}

void EntityCollection::Clear() {
	if (Engine::Instance()->CanAddOrRemoveEntities()) {
		// Can safely remove all the entities instantly
		m_Entities.clear();
	} else {
		// Must queue clear for later
		m_ClearQueued = true;
	}
}

std::shared_ptr<Entity>& EntityCollection::operator[](const int index) {
	return m_Entities[index];
}

std::vector<std::shared_ptr<Entity>>& EntityCollection::GetEntities() {
	return m_Entities;
}

std::vector<std::shared_ptr<Entity>>& EntityCollection::GetActiveEntities() {
	return m_ActiveEntities;
}

std::vector<std::shared_ptr<Entity>>& EntityCollection::GetInactiveEntities() {
	return m_InactiveEntities;
}

std::vector<std::shared_ptr<StaticBody>>& EntityCollection::GetStaticBodies() {
	return m_StaticBodies;
}

std::vector<std::shared_ptr<RigidBody>>& EntityCollection::GetRigidBodies() {
	return m_RigidBodies;
}

std::shared_ptr<EntityCollection> EntityCollection::Create() {
	auto collection = std::dynamic_pointer_cast<EntityCollection>(
		std::make_shared<MakeSharedEnabler>());

	collection->m_Id = Engine::Instance()->RegisterEntityCollection(collection);

	return collection;
}

void EntityCollection::Delete(int id) {
	Engine::Instance()->UnregisterEntityCollection(id);
}

void EntityCollection::RegisterStaticBody(
	std::shared_ptr<StaticBody> staticBody) {
	m_StaticBodies.push_back(staticBody);
}

void EntityCollection::RegisterRigidBody(std::shared_ptr<RigidBody> rigidBody) {
	m_RigidBodies.push_back(rigidBody);
}

void EntityCollection::UnregisterStaticBody(
	std::shared_ptr<StaticBody> staticBody) {
	auto it =
		std::find(m_StaticBodies.begin(), m_StaticBodies.end(), staticBody);

	ASSERT(it != m_StaticBodies.end());
	int index = std::distance(m_StaticBodies.begin(), it);

	m_StaticBodies.erase(m_StaticBodies.begin() + index);
}

void EntityCollection::UnregisterRigidBody(
	std::shared_ptr<RigidBody> rigidBody) {
	auto it = std::find(m_RigidBodies.begin(), m_RigidBodies.end(), rigidBody);

	ASSERT(it != m_RigidBodies.end());
	int index = std::distance(m_RigidBodies.begin(), it);

	m_RigidBodies.erase(m_RigidBodies.begin() + index);
}

void EntityCollection::SetEntityActive(std::shared_ptr<Entity> entity,
									   bool active) {
	if (entity->GetState() != EntityState::Normal) return;

	if (active) {
		UnregisterEntityInactive(entity);
		RegisterEntityActive(entity);

		if (entity->HasBody()) {
			if (entity->m_Components.count(EngineComponentType::StaticBody)) {
				RegisterStaticBody(std::dynamic_pointer_cast<StaticBody>(
					entity->m_Components[EngineComponentType::StaticBody]));
				// m_StaticBodies.emplace_back(entity);
			} else {
				RegisterRigidBody(std::dynamic_pointer_cast<RigidBody>(
					entity->m_Components[EngineComponentType::RigidBody]));
			}
		}
	} else {
		UnregisterEntityActive(entity);
		RegisterEntityInactive(entity);

		if (entity->HasBody()) {
			if (entity->m_Components.count(EngineComponentType::StaticBody)) {
				UnregisterStaticBody(std::dynamic_pointer_cast<StaticBody>(
					entity->m_Components[EngineComponentType::StaticBody]));
			} else {
				UnregisterRigidBody(std::dynamic_pointer_cast<RigidBody>(
					entity->m_Components[EngineComponentType::RigidBody]));
			}
		}
	}
}

void EntityCollection::RegisterEntityActive(std::shared_ptr<Entity> entity) {
	m_ActiveEntities.push_back(entity);
}

void EntityCollection::RegisterEntityInactive(std::shared_ptr<Entity> entity) {
	m_InactiveEntities.push_back(entity);
}

void EntityCollection::UnregisterEntityActive(std::shared_ptr<Entity> entity) {
	auto it =
		std::find(m_ActiveEntities.begin(), m_ActiveEntities.end(), entity);

	ASSERT(it != m_ActiveEntities.end());
	int index = std::distance(m_ActiveEntities.begin(), it);

	m_ActiveEntities.erase(m_ActiveEntities.begin() + index);
}

void EntityCollection::UnregisterEntityInactive(
	std::shared_ptr<Entity> entity) {
	auto it =
		std::find(m_InactiveEntities.begin(), m_InactiveEntities.end(), entity);

	ASSERT(it != m_InactiveEntities.end());
	int index = std::distance(m_InactiveEntities.begin(), it);

	m_InactiveEntities.erase(m_InactiveEntities.begin() + index);
}

void EntityCollection::RemoveInternal(std::shared_ptr<Entity> entity,
									  int index) {
	entity->m_Collection = NULL;
	if (Engine::Instance()->CanAddOrRemoveEntities()) {
		// Can safely remove the entity instantly
		RemoveEntity(entity, index);
	} else {
		// Must queue entity to be removed later
		entity->SetState(EntityState::QueuedForDeletion);
		m_RemoveQueue.emplace_back(std::make_pair(entity, index));
	}
}

void EntityCollection::AddEntity(std::shared_ptr<Entity>&& entity) {
	if (entity->HasBody() && entity->IsActive()) {
		if (entity->m_Components.count(EngineComponentType::StaticBody)) {
			RegisterStaticBody(std::dynamic_pointer_cast<StaticBody>(
				entity->m_Components[EngineComponentType::StaticBody]));
			// m_StaticBodies.emplace_back(entity);
		} else {
			RegisterRigidBody(std::dynamic_pointer_cast<RigidBody>(
				entity->m_Components[EngineComponentType::RigidBody]));
		}
	}

	if (entity->IsActive()) {
		RegisterEntityActive(entity);
	} else {
		RegisterEntityInactive(entity);
	}

	entity->SetState(EntityState::Normal);
	m_Entities.emplace_back(std::move(entity));
}

void EntityCollection::RemoveEntity(std::shared_ptr<Entity> entity, int index) {
	if (entity->HasBody() && entity->IsActive()) {
		if (entity->m_Components.count(EngineComponentType::StaticBody)) {
			UnregisterStaticBody(std::dynamic_pointer_cast<StaticBody>(
				entity->m_Components[EngineComponentType::StaticBody]));
		} else {
			UnregisterRigidBody(std::dynamic_pointer_cast<RigidBody>(
				entity->m_Components[EngineComponentType::RigidBody]));
		}
	}

	if (entity->IsActive()) {
		UnregisterEntityActive(entity);
	} else {
		UnregisterEntityInactive(entity);
	}

	entity->Cleanup();
	m_Entities.erase(m_Entities.begin() + index);
}

struct EntityCollection::MakeSharedEnabler : public EntityCollection {
	MakeSharedEnabler() : EntityCollection() {}
};