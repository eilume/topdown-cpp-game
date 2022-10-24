#include "game/components/game_manager.h"

#include <iostream>

#include "engine/component.h"
#include "engine/components/camera.h"
#include "engine/components/renderables.h"
#include "engine/engine.h"
#include "engine/entity.h"
#include "game/component.h"
#include "utils.h"

GameManager::GameManager()
	: Component(GameComponentType::GameManager),
	  m_GameOver(false),
	  m_Score(0),
	  m_AliveDuration(0) {}

void GameManager::Setup() {
	ASSERT(Engine::Instance()->GetCamera()->GetEntity()->HasComponent(
		EngineComponentType::RenderRect));
	damageFlashRect =
		Engine::Instance()->GetCamera()->GetEntity()->GetComponent<RenderRect>(
			EngineComponentType::RenderRect);

	m_AliveTimerThread = std::thread(&GameManager::AliveTimerThread, this);

	std::cout << "\n-================= Start! =================-" << std::endl;
	std::cout << "   Objective: Try and survive for as long\n              as "
				 "you can!"
			  << std::endl
			  << std::endl;
	std::cout << "   Controls:  WASD -> Movement\n              Arrow Keys -> "
				 "Fire Direction"
			  << std::endl
			  << std::endl;
}

void GameManager::Cleanup() {
	m_GameOver = true;
	m_AliveTimerThread.join();

	m_AutoQuitThread.detach();
}

void GameManager::FixedUpdate() {
	damageFlashRect->fillColor.a =
		std::clamp(damageFlashRect->fillColor.a - 5, 0, 255);
}

void GameManager::AddScore() {
	m_Score++;
	std::cout << "[Score]: Enemy Defeated!" << std::endl;
	std::cout << "         Current Score: "
			  << Utils::PrePadString(std::to_string(m_Score), 3, '0')
			  << std::endl;
}

void GameManager::GameOver() {
	{
		std::unique_lock<std::mutex> lk(m_AliveMtx);
		m_GameOver = true;
		m_AliveTimerCondVar.wait(lk);
		m_AliveTimerThread.join();
	}

	std::cout << "\n-=============== Game Over! ===============-" << std::endl;
	std::cout << "   You survived for: " << m_AliveDuration << " seconds!"
			  << std::endl;
	std::cout << "   Final Score: "
			  << Utils::PrePadString(std::to_string(m_Score), 3, '0')
			  << std::endl
			  << std::endl;

	m_AutoQuitThread = std::thread([]() {
		std::this_thread::sleep_for(std::chrono::seconds(3));

		Engine::Instance()->running = false;
	});
}

void GameManager::AliveTimerThread() {
	while (!m_GameOver) {
		m_AliveMtx.lock();
		m_AliveDuration += 0.1;
		// std::cout << m_AliveDuration << std::endl;
		m_AliveMtx.unlock();

		std::this_thread::sleep_for(std::chrono::milliseconds(100));
	}

	m_AliveTimerCondVar.notify_one();
}