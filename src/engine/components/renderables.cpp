#include "engine/components/renderables.h"

#include <SDL.h>

#include <algorithm>
#include <iostream>

#include "config.h"
#include "engine/components/camera.h"
#include "engine/engine.h"
#include "engine/entity.h"
#include "engine/mathutils.h"
#include "engine/physics.h"

Renderable::Renderable(ComponentType type, RenderMode renderMode,
					   SDL_Color fillColor, SDL_Color outlineColor, int order)
	: Component(type),
	  renderMode(renderMode),
	  fillColor(fillColor),
	  outlineColor(outlineColor),
	  order(order) {}

void Renderable::Render() {
	if (renderMode == RenderMode::None) return;

	if (renderMode != RenderMode::OutlineOnly) {
		RenderFill();
	}
	if (renderMode != RenderMode::FillOnly) {
		RenderOutline();
	}
}

RenderRect::RenderRect(RenderMode renderMode, SDL_Color fillColor,
					   SDL_Color outlineColor, int order)
	: Renderable(EngineComponentType::RenderRect, renderMode, fillColor,
				 outlineColor, order) {}

void RenderRect::RenderFill() const {
	AABB* camAABB = &Engine::Instance()->GetCamera()->GetEntity()->visualAABB;

	AABB aabb = GetEntity()->visualAABB;

	// Check if this can be culled
	if (!AABB::CheckIntersection(&aabb, camAABB)) return;

	Vec2 camOffset = camAABB->pos - camAABB->halfSize;
	aabb.pos -= camOffset;

	Engine::Instance()->AddRenderOp(
		std::dynamic_pointer_cast<RenderOp>(std::make_shared<RenderOpRectFill>(
			(SDL_FRect){aabb.pos.x - aabb.halfSize.x,
						aabb.pos.y - aabb.halfSize.y, aabb.halfSize.x * 2,
						aabb.halfSize.y * 2},
			fillColor, order)));
}

void RenderRect::RenderOutline() const {
	AABB* camAABB = &Engine::Instance()->GetCamera()->GetEntity()->visualAABB;

	AABB aabb = GetEntity()->visualAABB;

	// Check if this can be culled
	if (!AABB::CheckIntersection(&aabb, camAABB)) return;

	Vec2 camOffset = camAABB->pos - camAABB->halfSize;
	aabb.pos -= camOffset;

	Vec2 min, max;
	aabb.GetMinMax(min, max);

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

	Engine::Instance()->AddRenderOp(std::dynamic_pointer_cast<RenderOp>(
		std::make_shared<RenderOpRectOutline>(points, outlineColor, order)));
}