#pragma once

#include <SDL.h>

// TODO: use template to make data generic?
class ActionData {
   public:
	ActionData();

	void Start(bool value);
	void Stop(bool value);

	float GetStartTime() const;
	float GetEndTime() const;

	float GetDuration() const;

	bool IsHeld() const;

	bool InPhase() const;

	bool IsStarted() const;
	bool IsPerformed() const;
	bool IsStopped() const;

	bool IsNew() const;
	bool IsExpired() const;

	bool GetValue() const;

   private:
	bool m_Value;
	float m_StartTime;
	float m_EndTime;
};

class Input {
   public:
	Input();

	bool GetKey(int scancode);

	void SetKeyUp(int scancode);
	void SetKeyDown(int scancode);

   private:
	bool m_KeyDown[SDL_NUM_SCANCODES];
};