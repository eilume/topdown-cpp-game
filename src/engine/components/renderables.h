#pragma once

#include <SDL.h>

#include "engine/component.h"

enum class RenderMode {
	None,
	FillOnly,
	OutlineOnly,
	Both,
};

class Renderable : public Component {
   protected:
	Renderable(ComponentType type, RenderMode renderMode = RenderMode::FillOnly,
			   SDL_Color fillColor = Color::White,
			   SDL_Color outlineColor = Color::White, int order = 0);

   public:
	void Render() override;

   protected:
	virtual void RenderFill() const = 0;
	virtual void RenderOutline() const = 0;

   public:
	RenderMode renderMode;

	SDL_Color fillColor;
	SDL_Color outlineColor;

	int order;
};

class RenderRect : public Renderable {
   public:
	RenderRect(RenderMode renderMode = RenderMode::FillOnly,
			   SDL_Color fillColor = Color::White,
			   SDL_Color outlineColor = Color::White, int order = 0);

   protected:
	void RenderFill() const override;
	void RenderOutline() const override;
};

// TODO: add rotate rect (via SDL_RenderGeometry())