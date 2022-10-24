#include "engine/renderop.h"

#include "engine/engine.h"

RenderOp::RenderOp(int order) : m_Order(order) {}
RenderOp::~RenderOp() {}

RenderOpRectFill::RenderOpRectFill(SDL_FRect rect, SDL_Color color, int order)
	: RenderOp(order), m_Rect(rect), m_Color(color) {}

void RenderOpRectFill::Execute() const {
	SDL_SetRenderDrawColor(Engine::Instance()->renderer, m_Color.r, m_Color.g,
						   m_Color.b, m_Color.a);
	SDL_RenderFillRectF(Engine::Instance()->renderer, &m_Rect);
}

RenderOpRectOutline::RenderOpRectOutline(std::shared_ptr<SDL_FPoint[]> points,
										 SDL_Color color, int order)
	: RenderOp(order), m_Points(points), m_Color(color) {}

void RenderOpRectOutline::Execute() const {
	SDL_SetRenderDrawColor(Engine::Instance()->renderer, m_Color.r, m_Color.g,
						   m_Color.b, m_Color.a);
	SDL_RenderDrawLinesF(Engine::Instance()->renderer, m_Points.get(), 5);
}