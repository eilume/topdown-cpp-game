#include <future>

#include "config.h"
#include "engine/engine.h"
#include "game/game.h"

int main(int argc, char* argv[]) {
	int engineResult = Engine::Instance()->Setup(argc, argv);
	if (engineResult > 0) return engineResult;

	Engine::Instance()->init = &Game::Init;
	Engine::Instance()->cleanup = &Game::Cleanup;

	Engine::Instance()->processInput = &Game::ProcessInput;
	Engine::Instance()->preFixedUpdate = &Game::PreFixedUpdate;
	Engine::Instance()->fixedUpdate = &Game::FixedUpdate;
	Engine::Instance()->postFixedUpdate = &Game::PostFixedUpdate;
	Engine::Instance()->update = &Game::Update;
	Engine::Instance()->render = &Game::Render;
	Engine::Instance()->submitRender = &Game::SubmitRender;

	Engine::Instance()->idling = &Game::Idling;

	Engine::Instance()->Run();
	Engine::Instance()->Cleanup();

	return 0;
}