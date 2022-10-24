#include "game/component.h"

#include "game/components/player.h"

const ComponentType GameComponentType::Player = GameComponentType(0);
const ComponentType GameComponentType::Bullet = GameComponentType(1);
const ComponentType GameComponentType::Enemy = GameComponentType(2);
const ComponentType GameComponentType::EnemyManager = GameComponentType(3);
const ComponentType GameComponentType::GameManager = GameComponentType(4);