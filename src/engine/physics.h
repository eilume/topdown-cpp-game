#pragma once

#include <SDL.h>

#include "engine/components/physics.h"
#include "engine/types/entity_collection.h"
#include "engine/types/vec2.h"

// Since I feel like writing a physics engine isn't really the focus of this
// project, I decided to port (and slightly extend) a very simple C physics
// engine from here:
// https://github.com/Falconerd/engine-from-scratch/blob/rec/src/engine/physics/physics.c

// They also have an excellent WIP YouTube series for creating a more
// feature-rich engine from scratch (in plain C + SDL + OpenGL!) here:
// https://youtube.com/playlist?list=PLYokS5qr7lSsvgemrTwMSrQsdk4BRqJU6

class Engine;

struct Hit {
	bool isHit;
	float time;
	Vec2 pos;
	Vec2 normal;
	std::shared_ptr<Body> hitBody;
};

struct AABB {
	Vec2 pos;
	Vec2 halfSize;

	void GetMinMax(Vec2& min, Vec2& max) const;

	void RenderFill(SDL_Color color);
	void RenderOutline(SDL_Color color);

	Hit RayIntersect(Vec2 position, Vec2 mag);
	bool PointIntersect(Vec2 point);

	static AABB GetMinkowskiDifference(AABB* a, AABB* b);
	static bool CheckIntersection(AABB* a, AABB* b);
	static Vec2 GetPenetration(AABB* minkowski);
};

class Physics {
   public:
	static void Update();

   private:
	static Hit SweepStaticBodies(std::shared_ptr<RigidBody> rigidBody,
								 Vec2 scaledVel);

	static Hit SweepRigidBodies(std::shared_ptr<RigidBody> rigidBody, Vec2 vel,
								double velScale);

	static void UpdateSweepResult(Hit* result, std::shared_ptr<Body> hitBody,
								  AABB subject, AABB obstacle, Vec2 scaledVel,
								  uint8_t subjectCollisionMask,
								  uint8_t obstacleCollisionLayer);

	static void SweepResponse(std::shared_ptr<RigidBody> rigidBody, Vec2 vel,
							  double velScale);

	static void StationaryResponse(std::shared_ptr<RigidBody> rigidBody);
};