#pragma once

#include <string>

// The glue for the specific game components

class Engine;

namespace Game {
bool Init();
void Cleanup();

void ProcessInput();
void PreFixedUpdate();
void FixedUpdate();
void PostFixedUpdate();
void Update();
void Render();
void SubmitRender();
void Idling();

bool LoadLevel(std::string path);
}  // namespace Game