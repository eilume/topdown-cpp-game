#pragma once

#include "config.h"
#include "engine/component.h"

class GameComponentType : public ComponentType {
   private:
	GameComponentType(int id)
		: ComponentType(id + Config::GameComponentIdOffset) {
		// Ensure there is separation between engine and game components
		ASSERT(id + Config::GameComponentIdOffset >=
			   Config::GameComponentIdOffset);
	}

   public:
	static const ComponentType Player;
	static const ComponentType Bullet;
	static const ComponentType Enemy;
	static const ComponentType EnemyManager;
	static const ComponentType GameManager;
};