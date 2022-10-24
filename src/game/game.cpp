#include "game/game.h"

#include <SDL.h>

#include <fstream>
#include <iostream>
#include <memory>

#include "config.h"
#include "engine/components/camera.h"
#include "engine/components/physics.h"
#include "engine/components/renderables.h"
#include "engine/engine.h"
#include "engine/entity.h"
#include "engine/physics.h"
#include "engine/types/entity_collection.h"
#include "game/components/enemy_manager.h"
#include "game/components/game_manager.h"
#include "game/components/player.h"

std::shared_ptr<EntityCollection> entities;

#define EntityFunctionCall(FuncName)                                          \
	for (size_t i = 0; i < Engine::Instance()->GetAllActiveEntities().size(); \
		 ++i) {                                                               \
		auto &entity = Engine::Instance()->GetAllActiveEntities()[i];         \
		if (!entity->CanBeUsed()) continue;                                   \
		entity->FuncName();                                                   \
	}

namespace Game {

bool Init() {
	entities = EntityCollection::Create();

	auto camera = std::make_shared<Entity>(
		(Vec2){0, 0},
		(Vec2){Config::ScreenWidth / 2.0, Config::ScreenHeight / 2.0});
	camera->AddComponent(std::make_shared<Camera>());
	camera->AddComponent(std::make_shared<RenderRect>(
		RenderMode::FillOnly, Color::SetAlpha(Color::Red, 0), Color::White, 1));

	entities->Add(std::move(camera));

	if (!LoadLevel("levels/level3.txt")) return false;

	auto gameManagerEntity = std::make_shared<Entity>(Vec2(), Vec2());
	gameManagerEntity->name = "GameManager";
	gameManagerEntity->AddComponent(std::make_shared<GameManager>());

	entities->Add(std::move(gameManagerEntity));

	auto playerEntity = std::make_shared<Entity>(Vec2(), Vec2());
	playerEntity->name = "Player";
	playerEntity->AddComponent(
		std::make_shared<RigidBody>(0b00000001, 0b00000111));
	playerEntity->AddComponent(std::make_shared<RenderRect>());
	playerEntity->AddComponent(std::make_shared<Player>());

	entities->Add(std::move(playerEntity));

	auto enemyManagerEntity =
		std::make_shared<Entity>((Vec2){0, 0}, (Vec2){0, 0});
	enemyManagerEntity->AddComponent(std::make_shared<EnemyManager>());

	entities->Add(std::move(enemyManagerEntity));

	return true;
}

void Cleanup() {}

void ProcessInput() {
	while (SDL_PollEvent(&Engine::Instance()->event) > 0) {
		switch (Engine::Instance()->event.type) {
			case SDL_QUIT:
				Engine::Instance()->running = false;
				break;

			case SDL_KEYDOWN:
				switch (Engine::Instance()->event.key.keysym.scancode) {
					case SDL_SCANCODE_ESCAPE:
					case SDL_SCANCODE_Q:
						Engine::Instance()->running = false;
						break;

					case SDL_SCANCODE_I:
						Engine::Instance()->doInterpolation =
							!Engine::Instance()->doInterpolation;
						break;

					default:
						break;
				}

				Engine::Instance()->GetInput()->SetKeyDown(
					Engine::Instance()->event.key.keysym.scancode);
				break;

			case SDL_KEYUP:
				Engine::Instance()->GetInput()->SetKeyUp(
					Engine::Instance()->event.key.keysym.scancode);
				break;

			case SDL_MOUSEWHEEL:
				// TODO: set input state for mouse
				break;

			default:
				break;
		}
	}

	// TODO: set input state for mouse in 'Engine::Instance()->GetInput()'
	// int mouseX, mouseY;
	// SDL_GetMouseState(&mouseX, &mouseY);
}

void SetLastPositions() {
	for (size_t i = 0; i < Engine::Instance()->GetAllActiveEntities().size();
		 ++i) {
		auto &entity = Engine::Instance()->GetAllActiveEntities()[i];
		if (!entity->CanBeUsed()) continue;
		entity->lastPos = entity->aabb.pos;
	}
}

void PreFixedUpdate() {
	for (auto collection : Engine::Instance()->GetEntityCollections()) {
		collection->ProcessRemoveQueue();
		collection->ProcessAddQueue();
	}

	// clang-format off
	EntityFunctionCall(PreFixedUpdate)

	SetLastPositions();
	// clang-format on
}

void FixedUpdate() {
	EntityFunctionCall(FixedUpdate)

		Physics::Update();
}

void PostFixedUpdate() { EntityFunctionCall(PostFixedUpdate) }

void Update() {
	// clang-format off
	EntityFunctionCall(PreUpdate)
	EntityFunctionCall(Update)
	EntityFunctionCall(PostUpdate)
	// clang-format on
}

void Render() { EntityFunctionCall(Render) }

void SubmitRender() {
	// SDL_SetRenderDrawColor(Engine::Instance()->renderer, 247, 244,
	// 240, 255);
	SDL_SetRenderDrawColor(Engine::Instance()->renderer, 8, 11, 15, 255);
	SDL_RenderClear(Engine::Instance()->renderer);
	Engine::Instance()->ExecuteRenderOps();

	// Swap buffer for presentation
	SDL_RenderPresent(Engine::Instance()->renderer);
}

void Idling() {
	SDL_SetRenderDrawColor(Engine::Instance()->renderer, 8, 11, 15, 255);
	SDL_RenderClear(Engine::Instance()->renderer);
	SDL_RenderPresent(Engine::Instance()->renderer);
}

void CreateBarrier(Vec2 pos, Vec2 halfSize) {
	auto barrierEntity = std::make_shared<Entity>(pos, halfSize);
	barrierEntity->AddComponent(std::make_shared<StaticBody>(0b00000010));
	barrierEntity->AddComponent(std::make_shared<RenderRect>(
		RenderMode::FillOnly, Color::SetAlpha(Color::VividPink, 127),
		Color::VividPink));

	entities->Add(std::move(barrierEntity));
}

void CreateObstacle(Vec2 pos, Vec2 halfSize) {
	auto obstacleEntity = std::make_shared<Entity>(pos, halfSize);
	obstacleEntity->AddComponent(std::make_shared<StaticBody>(0b00000010));
	obstacleEntity->AddComponent(std::make_shared<RenderRect>(
		RenderMode::FillOnly, Color::SetAlpha(Color::Orange, 127),
		Color::Orange));

	entities->Add(std::move(obstacleEntity));
}

void CreateGrid(Vec2 pos, Vec2 halfSize) {
	auto gridEntity = std::make_shared<Entity>(pos, halfSize);
	gridEntity->AddComponent(
		std::make_shared<RenderRect>(RenderMode::OutlineOnly, Color::White,
									 Color::SetAlpha(Color::White, 31)));

	entities->Add(std::move(gridEntity));
}

bool LoadLevel(std::string path) {
	std::ifstream levelFile(path);

	if (!levelFile.is_open()) {
		// Couldn't open file?
		std::cerr << "Couldn't load level file: '" << path << "'!" << std::endl;
		return false;
	}

	Vec2 dimensions;
	bool isFirstLine = true;
	int lastChar = ' ';
	std::vector<std::vector<char>> fileData;
	fileData.push_back(std::vector<char>());
	while (levelFile) {
		int c = levelFile.get();
		if (c == -1) break;

		if (c != '\n') {
			if (isFirstLine) dimensions.x++;

			fileData[dimensions.y].push_back(c);
		} else {
			dimensions.y++;
			fileData.push_back(std::vector<char>());
			isFirstLine = false;
		}

		lastChar = c;
	}

	if (lastChar != '!') {
		// File didn't end correctly
		std::cerr << "Level file ('" << path << "'), didn't end correctly!"
				  << std::endl;
		return false;
	}

	constexpr double HalfUnitSize = Config::UnitSize / 2.0;
	const Vec2 scaledDimensions = dimensions * Config::UnitSize;
	const Vec2 halfScaledDimensions = scaledDimensions / 2.0;
	const Vec2 posOffset = (scaledDimensions - Config::UnitSize) / 2.0;

	// Fill level
	for (int x = 0; x < dimensions.x; ++x) {
		for (int y = 0; y < dimensions.y; ++y) {
			Vec2 pos =
				(((Vec2(x, y) / dimensions) * 2) - 1) * halfScaledDimensions +
				HalfUnitSize;
			if (fileData[y][x] == 'b') {
				CreateBarrier(pos, Vec2(HalfUnitSize));
			} else if (fileData[y][x] == 'o') {
				CreateObstacle(pos, Vec2(HalfUnitSize));
			} else if (fileData[y][x] == '.') {
				CreateGrid(pos, Vec2(HalfUnitSize));
			}
		}
	}

	constexpr float borderThickness =
		std::max(Config::ScreenWidth, Config::ScreenHeight);
	constexpr float borderHalfThickness = borderThickness / 2.0;

	// Create barrier around level
	CreateBarrier(
		Vec2(halfScaledDimensions.x + borderHalfThickness, 0),
		Vec2(borderHalfThickness, halfScaledDimensions.y + borderThickness));
	CreateBarrier(Vec2(0, halfScaledDimensions.y + borderHalfThickness),
				  Vec2(halfScaledDimensions.x, borderHalfThickness));
	CreateBarrier(
		Vec2(-halfScaledDimensions.x - borderHalfThickness, 0),
		Vec2(borderHalfThickness, halfScaledDimensions.y + borderThickness));
	CreateBarrier(Vec2(0, -halfScaledDimensions.y - borderHalfThickness),
				  Vec2(halfScaledDimensions.x, borderHalfThickness));

	// Fake loading time for testing...
	// std::this_thread::sleep_for(std::chrono::milliseconds(2000));

	return true;
}
}  // namespace Game
