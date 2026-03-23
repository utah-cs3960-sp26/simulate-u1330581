#pragma once

#include <string>

#include <SDL3/SDL.h>

#include "physics/PhysicsEngine.h"

class Renderer {
public:
    Renderer();
    ~Renderer();

    Renderer(const Renderer&) = delete;
    Renderer& operator=(const Renderer&) = delete;

    /** Draws the current simulation frame and start button state. */
    void Render(const PhysicsEngine& engine, bool running, bool hoverButton) const;

    /** Returns true when the point lies within the start button. */
    bool IsInsideButton(float x, float y) const;

private:
    /** Draws a filled circle using horizontal scan lines. */
    void DrawFilledCircle(double centerX, double centerY, double radius, SDL_Color color) const;

    /** Draws a simple rounded button for the start action. */
    void DrawButton(bool running, bool hoverButton) const;

    /** Draws a minimal title using block segments. */
    void DrawLabel(double x, double y, const std::string& text, SDL_Color color) const;

    SDL_Window* window_ {nullptr};
    SDL_Renderer* renderer_ {nullptr};
    SDL_FRect buttonRect_ {40.0f, 40.0f, 180.0f, 56.0f};
};
