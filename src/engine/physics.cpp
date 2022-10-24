#include "engine/physics.h"

#include <algorithm>
#include <iostream>

#include "config.h"
#include "engine/engine.h"
#include "engine/entity.h"

void AABB::GetMinMax(Vec2& min, Vec2& max) const {
	min = pos - halfSize;
	max = pos + halfSize;
}

void AABB::RenderFill(SDL_Color color) {
	Vec2 min, max;
	GetMinMax(min, max);

	Engine::Instance()->AddRenderOp(std::move(
		std::dynamic_pointer_cast<RenderOp>(std::make_shared<RenderOpRectFill>(
			(SDL_FRect){min.x, min.y, halfSize.x * 2, halfSize.y * 2},
			color))));
}

void AABB::RenderOutline(SDL_Color color) {
	Vec2 min, max;
	GetMinMax(min, max);

	// Offset outline by 1px
	max -= 1.0;

	std::shared_ptr<SDL_FPoint[]> points(new SDL_FPoint[5]);
	points[0].x = min.x;
	points[0].y = min.y;
	points[1].x = max.x;
	points[1].y = min.y;
	points[2].x = max.x;
	points[2].y = max.y;
	points[3].x = min.x;
	points[3].y = max.y;
	points[4].x = min.x;
	points[4].y = min.y;

	Engine::Instance()->AddRenderOp(
		std::move(std::dynamic_pointer_cast<RenderOp>(
			std::make_shared<RenderOpRectOutline>(points, color))));
}

Hit AABB::RayIntersect(Vec2 position, Vec2 mag) {
	Hit hit = {0};
	Vec2 min, max;
	GetMinMax(min, max);

	float lastEntry = -INFINITY;
	float firstExit = INFINITY;

	for (uint8_t i = 0; i < 2; ++i) {
		if (mag[i] != 0) {
			float t1 = (min[i] - position[i]) / mag[i];
			float t2 = (max[i] - position[i]) / mag[i];

			lastEntry = std::max(lastEntry, std::min(t1, t2));
			firstExit = std::min(firstExit, std::max(t1, t2));
		} else if (position[i] <= min[i] || position[i] >= max[i]) {
			return hit;
		}
	}

	if (firstExit > lastEntry && firstExit > 0 && lastEntry < 1 &&
		lastEntry > -1) {
		hit.pos = position + mag * lastEntry;

		hit.isHit = true;
		hit.time = lastEntry;

		Vec2 d = hit.pos - pos;
		Vec2 p = halfSize - d.Abs();

		if (p.x < p.y) {
			hit.normal.x = (d.x > 0) - (d.x < 0);
		} else {
			hit.normal.y = (d.y > 0) - (d.y < 0);
		}
	}

	return hit;
}

bool AABB::PointIntersect(Vec2 point) {
	Vec2 min, max;
	GetMinMax(min, max);

	return point.x >= min.x && point.x <= max.x && point.y >= min.y &&
		   point.y <= max.y;
}

AABB AABB::GetMinkowskiDifference(AABB* a, AABB* b) {
	return (AABB){a->pos - b->pos, a->halfSize + b->halfSize};
}

bool AABB::CheckIntersection(AABB* a, AABB* b) {
	Vec2 min, max;
	GetMinkowskiDifference(a, b).GetMinMax(min, max);

	return min.x <= 0 && max.x >= 0 && min.y <= 0 && max.y >= 0;
}

Vec2 AABB::GetPenetration(AABB* minkowski) {
	Vec2 min, max;
	minkowski->GetMinMax(min, max);

	float minDistance = fabsf(min.x);
	Vec2 result = {min.x, 0};

	if (fabsf(max.x) < minDistance) {
		minDistance = fabsf(max.x);
		result.x = max.x;
	}

	if (fabsf(min.y) < minDistance) {
		minDistance = fabsf(min.y);
		result.x = 0;
		result.y = min.y;
	}

	if (fabsf(max.y) < minDistance) {
		result.x = 0;
		result.y = max.y;
	}

	return result;
}

void Physics::Update() {
	for (size_t i = 0; i < Engine::Instance()->GetAllRigidBodies().size();
		 ++i) {
		auto& rigidBody = Engine::Instance()->GetAllRigidBodies()[i];
		double velScale = Engine::Instance()->GetTimeState()->GetFixedStep() /
						  Config::PhysicsIterations;

		for (int i = 0; i < Config::PhysicsIterations; ++i) {
			SweepResponse(rigidBody, rigidBody->vel, velScale);
			StationaryResponse(rigidBody);
		}
	}
}

Hit Physics::SweepStaticBodies(std::shared_ptr<RigidBody> rigidBody,
							   Vec2 scaledVel) {
	Hit result = {0};
	result.time = 0xBEEF;

	for (size_t i = 0; i < Engine::Instance()->GetAllStaticBodies().size();
		 ++i) {
		auto& staticBody = Engine::Instance()->GetAllStaticBodies()[i];
		UpdateSweepResult(&result, std::dynamic_pointer_cast<Body>(staticBody),
						  rigidBody->GetEntity()->aabb,
						  staticBody->GetEntity()->aabb, scaledVel,
						  rigidBody->collisionMask, staticBody->collisionLayer);
	}

	return result;
}

Hit Physics::SweepRigidBodies(std::shared_ptr<RigidBody> rigidBody, Vec2 vel,
							  double velScale) {
	Hit result = {0};
	result.time = 0xBEEF;

	for (size_t i = 0; i < Engine::Instance()->GetAllRigidBodies().size();
		 ++i) {
		auto& otherRigidBody = Engine::Instance()->GetAllRigidBodies()[i];
		if (rigidBody == otherRigidBody) continue;

		// Use relative velocity between the two bodies
		// Based on discussion:
		// https://www.gamedev.net/forums/topic/696688-sweep-test-with-two-moving-bodies/
		UpdateSweepResult(
			&result, std::dynamic_pointer_cast<Body>(otherRigidBody),
			rigidBody->GetEntity()->aabb, otherRigidBody->GetEntity()->aabb,
			(vel - otherRigidBody->vel) * velScale, rigidBody->collisionMask,
			otherRigidBody->collisionLayer);
	}

	return result;
}

void Physics::UpdateSweepResult(Hit* result, std::shared_ptr<Body> hitBody,
								AABB subject, AABB obstacle, Vec2 vel,
								uint8_t subjectCollisionMask,
								uint8_t obstacleCollisionLayer) {
	if ((subjectCollisionMask & obstacleCollisionLayer) == 0) return;

	AABB sumAABB = obstacle;
	sumAABB.halfSize += subject.halfSize;

	Hit hit = sumAABB.RayIntersect(subject.pos, vel);
	if (!hit.isHit) return;

	if (hit.time < result->time) {
		*result = hit;
	} else if (hit.time == result->time) {
		// Solve highest velocity axis first
		if (fabsf(vel.x) > fabsf(vel.y) && hit.normal.x != 0) {
			*result = hit;
		} else if (fabsf(vel.y) > fabsf(vel.x) && hit.normal.y != 0) {
			*result = hit;
		}
	}

	result->hitBody = hitBody;
}

void Physics::SweepResponse(std::shared_ptr<RigidBody> rigidBody, Vec2 vel,
							double velScale) {
	Vec2 scaledVel = vel * velScale;

	Hit hitStatic = SweepStaticBodies(rigidBody, scaledVel);
	Hit hitRigid = SweepRigidBodies(rigidBody, vel, velScale);

	if (hitRigid.isHit) {
		// TODO: do some kinda response

		rigidBody->GetEntity()->OnHit(&hitRigid);
	}

	AABB* rigidBodyAABB = &rigidBody->GetEntity()->aabb;
	if (hitStatic.isHit) {
		rigidBodyAABB->pos = hitStatic.pos;

		if (hitStatic.normal.x != 0) {
			rigidBodyAABB->pos.y += scaledVel.y;
			rigidBody->vel.x = 0;
		} else if (hitStatic.normal.y != 0) {
			rigidBodyAABB->pos.x += scaledVel.x;
			rigidBody->vel.y = 0;
		}

		rigidBody->GetEntity()->OnHit(&hitStatic);
	} else {
		rigidBodyAABB->pos += scaledVel;
	}
}

void Physics::StationaryResponse(std::shared_ptr<RigidBody> rigidBody) {
	for (size_t i = 0; i < Engine::Instance()->GetAllStaticBodies().size();
		 ++i) {
		auto& staticBody = Engine::Instance()->GetAllStaticBodies()[i];
		if ((rigidBody->collisionMask & staticBody->collisionLayer) == 0) {
			continue;
		}

		AABB* rigidBodyAABB = &rigidBody->GetEntity()->aabb;

		AABB box = AABB::GetMinkowskiDifference(&staticBody->GetEntity()->aabb,
												rigidBodyAABB);

		Vec2 min, max;
		box.GetMinMax(min, max);

		if (min.x <= 0 && max.x >= 0 && min.y <= 0 && max.y >= 0) {
			rigidBodyAABB->pos + AABB::GetPenetration(&box);
		}
	}
}
