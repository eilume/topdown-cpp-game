#include "engine/components/camera.h"

#include "engine/engine.h"
#include "utils.h"

Camera::Camera() : Component(EngineComponentType::Camera) {}

void Camera::Setup() {
	// Ensure this is the only camera
	ASSERT(Engine::Instance()->GetCamera() == nullptr);

	Engine::Instance()->SetCamera(shared_from_this());
}