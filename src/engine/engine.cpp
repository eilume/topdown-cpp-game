#include "engine/engine.h"

#include <iostream>
#include <string>

#include "config.h"
#include "engine/types/entity_collection.h"
#include "mathutils.h"
#include "utils.h"

Engine::Engine()
	: running(true),
	  m_ScheduledFixedUpdateTicks(true),
	  doInterpolation(true),
	  m_RenderingRenderBufferIndex(0),
	  m_PreparingRenderBufferIndex(0),
	  m_Stage(EngineStage::Idle),
	  m_IsCleanedUp(false) {}

Engine::~Engine() {
	// Ensure engine cleans up properly
	if (!m_IsCleanedUp) Cleanup();
}

std::shared_ptr<Engine> Engine::Instance() {
	static std::shared_ptr<Engine> instance = std::dynamic_pointer_cast<Engine>(
		std::make_shared<MakeSharedEnabler>());
	return instance;
}

int Engine::Setup(int argc, char* argv[]) {
	m_Stage = EngineStage::Setup;

	int sdlSetupResult = SetupSDL();
	if (sdlSetupResult > 0) return sdlSetupResult;

	m_TimeState = std::make_shared<TimeState>(SDL_GetPerformanceCounter(),
											  SDL_GetPerformanceFrequency(),
											  Config::FixedTimeStep);

	m_Input = std::make_shared<Input>();

	m_Stage = EngineStage::Idle;
	return 0;
}

void Engine::Run() {
	m_Stage = EngineStage::Init;
	std::future<bool> initFuture = std::async(init);
	while (true) {
		processInput();
		idling();

		if (initFuture.wait_for(std::chrono::seconds(0)) ==
			std::future_status::ready) {
			break;
		}
	}

	if (!initFuture.get()) return;

	m_Stage = EngineStage::Run;
	GameLoop();
}

void Engine::Cleanup() {
	m_Stage = EngineStage::Cleanup;

	cleanup();

	CleanupSDL();

	m_Stage = EngineStage::Idle;
	m_IsCleanedUp = true;
}

void Engine::AddRenderOp(std::shared_ptr<RenderOp> renderOp) {
	GetNextRenderBuffer().push_back(renderOp);
}

int Engine::SetupSDL() {
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS) < 0) {
		std::cout << "Error: SDL2 initialization failed! " << SDL_GetError()
				  << std::endl;
		return 1;
	}

	window = SDL_CreateWindow(Config::GameName.data(), SDL_WINDOWPOS_CENTERED,
							  SDL_WINDOWPOS_CENTERED, Config::ScreenWidth,
							  Config::ScreenHeight, SDL_WINDOW_SHOWN);
	if (window == NULL) {
		std::cout << "Error: SDL2 Window creation failed! " << SDL_GetError()
				  << std::endl;
		return 1;
	}

	renderer = SDL_CreateRenderer(
		window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	if (renderer == NULL) {
		std::cout << "Error: SDL2 Renderer creation failed! " << SDL_GetError()
				  << std::endl;
		return 1;
	}

	SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

	return 0;
}

void Engine::CleanupSDL() {
	SDL_DestroyRenderer(Engine::Instance()->renderer);
	SDL_DestroyWindow(Engine::Instance()->window);

	SDL_Quit();
}

void Engine::GameLoop() {
	while (running) {
		UpdateTick();
		submitRender();
	}
}

void Engine::ExecuteRenderOps() {
	// Execute current buffer
	std::sort(GetCurrentRenderBuffer().begin(), GetCurrentRenderBuffer().end(),
			  [](std::shared_ptr<RenderOp>& l, std::shared_ptr<RenderOp>& r) {
				  return l->GetOrder() < r->GetOrder();
			  });
	for (auto renderOp : GetCurrentRenderBuffer()) {
		renderOp->Execute();
	}
}

void Engine::UpdateTimeStates() {
	m_ScheduledFixedUpdateTicks =
		m_TimeState->Update(SDL_GetPerformanceCounter());
}

void Engine::UpdateTick() {
	m_Stage = EngineStage::Run;
	UpdateTimeStates();

	m_Stage = EngineStage::ProcessInput;
	processInput();

	if (m_ScheduledFixedUpdateTicks > 0) {
		m_Stage = EngineStage::PreFixedUpdate;
		preFixedUpdate();

		m_Stage = EngineStage::FixedUpdate;
		for (int i = 0; i < m_ScheduledFixedUpdateTicks; ++i) {
			fixedUpdate();
		}

		postFixedUpdate();
	}

	m_Stage = EngineStage::Update;
	update();

	m_Stage = EngineStage::Render;
	render();

	NextRenderBuffer();
}

void Engine::NextRenderBuffer() {
	// Update indexes
	m_RenderingRenderBufferIndex = m_PreparingRenderBufferIndex;
	m_PreparingRenderBufferIndex =
		(m_PreparingRenderBufferIndex + 1) % m_RenderBufferSize;

	// Clear buffer for new data to be added
	GetNextRenderBuffer().clear();
}

int Engine::RegisterEntityCollection(
	std::shared_ptr<EntityCollection> collection) {
	m_EntityCollections.push_back(collection);

	m_AllEntities.Add(&collection->GetEntities());
	m_AllActiveEntities.Add(&collection->GetActiveEntities());
	m_AllInactiveEntities.Add(&collection->GetInactiveEntities());
	m_AllStaticBodies.Add(&collection->GetStaticBodies());
	m_AllRigidBodies.Add(&collection->GetRigidBodies());

	return m_EntityCollections.size() - 1;
}

void Engine::UnregisterEntityCollection(int id) {
	auto it = std::find_if(
		m_EntityCollections.begin(), m_EntityCollections.end(),
		[id](std::shared_ptr<EntityCollection> c) { return c->GetId() == id; });

	ASSERT(it != m_EntityCollections.end());
	int index = std::distance(m_EntityCollections.begin(), it);

	m_AllEntities.RemoveAt(index);
	m_AllActiveEntities.RemoveAt(index);
	m_AllInactiveEntities.RemoveAt(index);
	m_AllStaticBodies.RemoveAt(index);
	m_AllRigidBodies.RemoveAt(index);

	m_EntityCollections.erase(m_EntityCollections.begin() + index);
}

struct Engine::MakeSharedEnabler : public Engine {
	MakeSharedEnabler() : Engine() {}
};