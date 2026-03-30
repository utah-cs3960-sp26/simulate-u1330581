#include "Renderer.h"

#include <algorithm>
#include <cmath>
#include <stdexcept>

#include "physics/Config.h"

namespace {
void SetColor(SDL_Renderer* renderer, SDL_Color color) {
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
}
}

Renderer::Renderer() {
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        throw std::runtime_error(SDL_GetError());
    }

    window_ = SDL_CreateWindow("Deterministic Physics Simulator",
        SimulationConfig::windowWidth,
        SimulationConfig::windowHeight,
        0);
    if (window_ == nullptr) {
        throw std::runtime_error(SDL_GetError());
    }

    renderer_ = SDL_CreateRenderer(window_, nullptr);
    if (renderer_ == nullptr) {
        throw std::runtime_error(SDL_GetError());
    }
}

Renderer::~Renderer() {
    if (renderer_ != nullptr) {
        SDL_DestroyRenderer(renderer_);
    }
    if (window_ != nullptr) {
        SDL_DestroyWindow(window_);
    }
    SDL_Quit();
}

void Renderer::Render(const PhysicsEngine& engine, bool running, bool hoverButton) const {
    SetColor(renderer_, SDL_Color {6, 6, 6, 255});
    SDL_RenderClear(renderer_);

    for (const Ball& ball : engine.GetSimulation().GetBalls()) {
        if (!ball.active && !ball.sleeping) {
            continue;
        }
        const SDL_Color color {ball.color.r, ball.color.g, ball.color.b, ball.color.a};
        DrawFilledCircle(ball.renderPosition.x, ball.renderPosition.y, ball.radius, color);
    }

    DrawButton(running, hoverButton);
    DrawLabel(44.0, 120.0, running ? "FORMING CREST" : "CLICK TO START", SDL_Color {245, 201, 23, 255});
    SDL_RenderPresent(renderer_);
}

bool Renderer::IsInsideButton(float x, float y) const {
    return x >= buttonRect_.x
        && x <= buttonRect_.x + buttonRect_.w
        && y >= buttonRect_.y
        && y <= buttonRect_.y + buttonRect_.h;
}

void Renderer::DrawFilledCircle(double centerX, double centerY, double radius, SDL_Color color) const {
    SetColor(renderer_, color);
    const int minY = static_cast<int>(std::floor(centerY - radius));
    const int maxY = static_cast<int>(std::ceil(centerY + radius));
    for (int y = minY; y <= maxY; ++y) {
        const double dy = static_cast<double>(y) - centerY;
        const double dx = std::sqrt(std::max(0.0, radius * radius - dy * dy));
        SDL_RenderLine(renderer_, centerX - dx, static_cast<double>(y), centerX + dx, static_cast<double>(y));
    }
}

void Renderer::DrawButton(bool running, bool hoverButton) const {
    const SDL_Color background = running
        ? SDL_Color {68, 27, 94, 255}
        : (hoverButton ? SDL_Color {196, 30, 58, 255} : SDL_Color {128, 10, 43, 255});
    SetColor(renderer_, background);
    SDL_RenderFillRect(renderer_, &buttonRect_);

    SetColor(renderer_, SDL_Color {245, 201, 23, 255});
    SDL_RenderRect(renderer_, &buttonRect_);
    DrawLabel(buttonRect_.x + 24.0, buttonRect_.y + 18.0, running ? "RUNNING" : "START", SDL_Color {255, 250, 245, 255});
}

void Renderer::DrawLabel(double x, double y, const std::string& text, SDL_Color color) const {
    SetColor(renderer_, color);
    const double scale = 3.0;
    const double glyphWidth = 4.0 * scale;
    double cursorX = x;

    auto drawBlock = [&](int gx, int gy, int gw, int gh) {
        SDL_FRect rect {
            static_cast<float>(cursorX + static_cast<double>(gx) * scale),
            static_cast<float>(y + static_cast<double>(gy) * scale),
            static_cast<float>(static_cast<double>(gw) * scale),
            static_cast<float>(static_cast<double>(gh) * scale)
        };
        SDL_RenderFillRect(renderer_, &rect);
    };

    for (char ch : text) {
        switch (ch) {
        case 'A': drawBlock(0, 1, 1, 5); drawBlock(3, 1, 1, 5); drawBlock(1, 0, 2, 1); drawBlock(1, 3, 2, 1); break;
        case 'C': drawBlock(1, 0, 3, 1); drawBlock(0, 1, 1, 4); drawBlock(1, 5, 3, 1); break;
        case 'G': drawBlock(1, 0, 3, 1); drawBlock(0, 1, 1, 4); drawBlock(1, 5, 3, 1); drawBlock(3, 3, 1, 2); drawBlock(2, 3, 1, 1); break;
        case 'I': drawBlock(0, 0, 4, 1); drawBlock(1, 1, 2, 4); drawBlock(0, 5, 4, 1); break;
        case 'K': drawBlock(0, 0, 1, 6); drawBlock(2, 2, 1, 1); drawBlock(3, 1, 1, 1); drawBlock(3, 3, 1, 1); drawBlock(2, 4, 1, 1); break;
        case 'L': drawBlock(0, 0, 1, 6); drawBlock(1, 5, 3, 1); break;
        case 'M': drawBlock(0, 0, 1, 6); drawBlock(3, 0, 1, 6); drawBlock(1, 1, 1, 1); drawBlock(2, 1, 1, 1); break;
        case 'N': drawBlock(0, 0, 1, 6); drawBlock(3, 0, 1, 6); drawBlock(1, 1, 1, 1); drawBlock(2, 2, 1, 1); break;
        case 'O': drawBlock(1, 0, 2, 1); drawBlock(0, 1, 1, 4); drawBlock(3, 1, 1, 4); drawBlock(1, 5, 2, 1); break;
        case 'R': drawBlock(0, 0, 1, 6); drawBlock(1, 0, 2, 1); drawBlock(3, 1, 1, 2); drawBlock(1, 3, 2, 1); drawBlock(2, 4, 1, 1); drawBlock(3, 5, 1, 1); break;
        case 'S': drawBlock(1, 0, 3, 1); drawBlock(0, 1, 1, 2); drawBlock(1, 2, 2, 1); drawBlock(3, 3, 1, 2); drawBlock(0, 5, 3, 1); break;
        case 'T': drawBlock(0, 0, 4, 1); drawBlock(1, 1, 2, 5); break;
        case 'U': drawBlock(0, 0, 1, 5); drawBlock(3, 0, 1, 5); drawBlock(1, 5, 2, 1); break;
        case ' ': break;
        default: drawBlock(0, 0, 4, 6); break;
        }
        cursorX += glyphWidth + scale;
        if (ch == ' ') {
            cursorX += scale * 2.0;
        }
    }
}
