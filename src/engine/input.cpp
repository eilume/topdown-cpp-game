#include "engine/input.h"

#include <iostream>

#include "engine/engine.h"
#include "utils.h"

ActionData::ActionData() : m_StartTime(-1), m_EndTime(-1), m_Value(false) {}

void ActionData::Start(bool value) {
	if (IsNew() == IsExpired()) return;

	m_StartTime = Engine::Instance()->GetTimeState()->GetTime();
	m_EndTime = -1;

	m_Value = value;
}

void ActionData::Stop(bool value) {
	if (IsNew() || IsExpired()) return;

	m_EndTime = Engine::Instance()->GetTimeState()->GetTime();

	m_Value = value;
}

float ActionData::GetStartTime() const { return m_StartTime; }
float ActionData::GetEndTime() const { return m_EndTime; };

float ActionData::GetDuration() const {
	return Engine::Instance()->GetTimeState()->GetTime() - m_StartTime;
};

bool ActionData::IsHeld() const { return m_StartTime != -1 && m_EndTime == -1; }

bool ActionData::InPhase() const {
	return IsStarted() || IsPerformed() || IsStopped();
}

bool ActionData::IsStarted() const {
	return Engine::Instance()->GetTimeState()->GetTime() == m_StartTime;
}
bool ActionData::IsPerformed() const {
	return Engine::Instance()->GetTimeState()->GetTime() != m_EndTime;
}
bool ActionData::IsStopped() const {
	return Engine::Instance()->GetTimeState()->GetTime() == m_EndTime;
}

bool ActionData::IsNew() const { return m_StartTime == -1 && m_EndTime == -1; }
bool ActionData::IsExpired() const {
	return m_EndTime != -1 &&
		   Engine::Instance()->GetTimeState()->GetTime() > m_EndTime;
}

bool ActionData::GetValue() const { return m_Value; }

#define CHECK_VALID_SCANCODE(x) ASSERT(x < SDL_NUM_SCANCODES)

Input::Input() {
	// Ensure initialized to false/0
	memset(m_KeyDown, false, sizeof m_KeyDown);
}

bool Input::GetKey(int scancode) {
	CHECK_VALID_SCANCODE(scancode);
	return m_KeyDown[scancode];
}

void Input::SetKeyUp(int scancode) {
	CHECK_VALID_SCANCODE(scancode);
	m_KeyDown[scancode] = false;
}
void Input::SetKeyDown(int scancode) {
	CHECK_VALID_SCANCODE(scancode);
	m_KeyDown[scancode] = true;
}