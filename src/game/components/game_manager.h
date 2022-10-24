#pragma once

#include <condition_variable>
#include <memory>
#include <mutex>
#include <thread>

#include "engine/component.h"
#include "engine/types/vec2.h"

class RenderRect;

class GameManager : public Component {
   public:
	GameManager();

	void Setup() override;
	void Cleanup() override;

	void FixedUpdate() override;

	void AddScore();
	void GameOver();

   private:
	void AliveTimerThread();

   public:
	std::shared_ptr<RenderRect> damageFlashRect;

   private:
	std::thread m_AliveTimerThread;
	std::thread m_AutoQuitThread;

	std::mutex m_AliveMtx;
	std::condition_variable m_AliveTimerCondVar;

	bool m_GameOver;

	int m_Score;
	double m_AliveDuration;
};