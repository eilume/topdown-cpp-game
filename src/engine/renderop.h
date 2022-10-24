#pragma once

#include <SDL.h>

#include <memory>

// Small containers of data + functions on how to render the data. This is
// intended for threaded rendering as this requires much less memory than
// storing a copied instance of the world simply for just rendering (as there is
// a lot of state data that is NOT used for rendering)

class Engine;

class RenderOp {
   protected:
	RenderOp(int order = 0);

   public:
	virtual ~RenderOp();

	virtual void Execute() const = 0;

	inline int GetOrder() const { return m_Order; }

   private:
	int m_Order;
};

class RenderOpRectFill : public RenderOp {
   public:
	RenderOpRectFill(SDL_FRect rect, SDL_Color color, int order = 0);

	void Execute() const override;

   private:
	SDL_FRect m_Rect;
	SDL_Color m_Color;
};

class RenderOpRectOutline : public RenderOp {
   public:
	RenderOpRectOutline(std::shared_ptr<SDL_FPoint[]> points, SDL_Color color,
						int order = 0);

	void Execute() const override;

   private:
	std::shared_ptr<SDL_FPoint[]> m_Points;
	SDL_Color m_Color;
};