#pragma once

#include <SDL.h>

#include <future>
#include <memory>
#include <thread>
#include <vector>

#include "engine/components/physics.h"
#include "engine/input.h"
#include "engine/renderop.h"
#include "engine/timestate.h"
#include "engine/types/proxy_vector.h"

// The engine is responsible for handling when each stage of the game loop is
// meant to occur, handling internal systems (such as SDL) and providing an easy
// to use API for the game to interface with

class Engine;
class EntityCollection;
class Camera;

// List of function types that are used by the engine and declared externally by
// the game
typedef bool GameInit();
typedef void GameCleanup();

typedef void GameProcessInput();
typedef void GamePreFixedUpdate();
typedef void GameFixedUpdate();
typedef void GamePostFixedUpdate();
typedef void GameUpdate();
typedef void GameRender();
typedef void GameSubmitRender();

typedef void GameIdling();

enum class EngineStage {
	Idle,
	Setup,
	Init,
	Run,
	ProcessInput,
	PreFixedUpdate,
	FixedUpdate,
	PostFixedUpdate,
	Update,
	Render,
	Cleanup,
};

enum class EngineRenderThreadState {
	Paused,
	Running,
	Stopping,
};

// A trick to allow for 'std::make_shared' to work with the private
// constructor is used!
// Source: https://stackoverflow.com/a/20961251

class Engine {
   private:
	Engine();
	~Engine();

   public:
	// Singleton engine
	static std::shared_ptr<Engine> Instance();

	int Setup(int argc, char *argv[]);
	void Run();
	void Cleanup();

	void ExecuteRenderOps();

	void AddRenderOp(std::shared_ptr<RenderOp> renderOp);

	inline std::shared_ptr<TimeState> GetTimeState() const {
		return m_TimeState;
	}
	inline std::shared_ptr<Input> GetInput() const { return m_Input; }

	inline std::shared_ptr<Camera> GetCamera() const { return m_Camera; }

	inline void SetCamera(std::shared_ptr<Camera> camera) { m_Camera = camera; }

	inline EngineStage GetStage() const { return m_Stage; }
	inline bool CanAddOrRemoveEntities() const {
		return m_Stage == EngineStage::Idle || m_Stage == EngineStage::Setup ||
			   m_Stage == EngineStage::Init || m_Stage == EngineStage::Run ||
			   m_Stage == EngineStage::Cleanup ||
			   m_Stage == EngineStage::PreFixedUpdate;
	}

	inline double GetInterpolation() const {
		return doInterpolation ? m_TimeState->GetFixedStepProgress() : 1.0;
	}

	inline std::vector<std::shared_ptr<EntityCollection>>
		&GetEntityCollections() {
		return m_EntityCollections;
	}

	inline ProxyVector<std::shared_ptr<Entity>> &GetAllEntities() {
		return m_AllEntities;
	}

	inline ProxyVector<std::shared_ptr<Entity>> &GetAllActiveEntities() {
		return m_AllActiveEntities;
	}

	inline ProxyVector<std::shared_ptr<Entity>> &GetAllInactiveEntities() {
		return m_AllInactiveEntities;
	}

	inline ProxyVector<std::shared_ptr<StaticBody>> &GetAllStaticBodies() {
		return m_AllStaticBodies;
	}

	inline ProxyVector<std::shared_ptr<RigidBody>> &GetAllRigidBodies() {
		return m_AllRigidBodies;
	}

   private:
	int SetupSDL();
	void CleanupSDL();

	void GameLoop();

	void UpdateTimeStates();

	void UpdateTick();

	void NextRenderBuffer();

	inline std::vector<std::shared_ptr<RenderOp>> &GetCurrentRenderBuffer() {
		return m_RenderBuffer[m_RenderingRenderBufferIndex];
	}

	inline std::vector<std::shared_ptr<RenderOp>> &GetNextRenderBuffer() {
		return m_RenderBuffer[m_PreparingRenderBufferIndex];
	}

	int RegisterEntityCollection(std::shared_ptr<EntityCollection> collection);
	void UnregisterEntityCollection(int id);

   public:
	GameInit *init;
	GameCleanup *cleanup;

	GameProcessInput *processInput;
	GamePreFixedUpdate *preFixedUpdate;
	GameFixedUpdate *fixedUpdate;
	GamePostFixedUpdate *postFixedUpdate;
	GameUpdate *update;
	GameRender *render;
	GameSubmitRender *submitRender;

	GameIdling *idling;

	SDL_Window *window;
	SDL_Renderer *renderer;
	SDL_Event event;

	bool running = true;
	bool doInterpolation = true;

   private:
	friend class EntityCollection;
	int m_ScheduledFixedUpdateTicks = 0;

	EngineStage m_Stage;
	bool m_IsSetupAndIdling;
	bool m_IsCleanedUp;

	std::shared_ptr<TimeState> m_TimeState;
	std::shared_ptr<Input> m_Input;

	// Entities/Components
	std::vector<std::shared_ptr<EntityCollection>> m_EntityCollections;

	// Proxy vectors providing easy indexing/iteration over all entity
	// collections registered with the engine
	ProxyVector<std::shared_ptr<Entity>> m_AllEntities;

	ProxyVector<std::shared_ptr<Entity>> m_AllActiveEntities;
	ProxyVector<std::shared_ptr<Entity>> m_AllInactiveEntities;

	ProxyVector<std::shared_ptr<StaticBody>> m_AllStaticBodies;
	ProxyVector<std::shared_ptr<RigidBody>> m_AllRigidBodies;

	// Singleton camera
	std::shared_ptr<Camera> m_Camera;

	// Rendering
	// Double-buffered render operations for threading
	static constexpr int m_RenderBufferSize = 2;
	std::vector<std::shared_ptr<RenderOp>> m_RenderBuffer[m_RenderBufferSize];
	int m_RenderingRenderBufferIndex;
	int m_PreparingRenderBufferIndex;

	std::thread m_IdlingThread;
	std::condition_variable m_IdlingCondVar;
	std::mutex m_IdlingMutex;

	struct MakeSharedEnabler;
};