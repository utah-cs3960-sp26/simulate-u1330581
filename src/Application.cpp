#include "Application.h"

#include <SDL3/SDL.h>

#include "SimulationFactory.h"
#include "physics/Config.h"

Application::Application()
    : engine_(SimulationFactory::CreateSimulation()) {
}

int Application::Run() {
    double accumulator = 0.0;
    std::uint64_t previousCounter = SDL_GetPerformanceCounter();

    while (!quit_) {
        SDL_Event event;
        float mouseX = 0.0f;
        float mouseY = 0.0f;
        SDL_GetMouseState(&mouseX, &mouseY);

        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT) {
                quit_ = true;
            } else if (event.type == SDL_EVENT_MOUSE_BUTTON_DOWN && event.button.button == SDL_BUTTON_LEFT) {
                if (!running_ && renderer_.IsInsideButton(event.button.x, event.button.y)) {
                    running_ = true;
                }
            }
        }

        const std::uint64_t currentCounter = SDL_GetPerformanceCounter();
        const double deltaSeconds = static_cast<double>(currentCounter - previousCounter)
            / static_cast<double>(SDL_GetPerformanceFrequency());
        previousCounter = currentCounter;

        accumulator += deltaSeconds;
        const double maxFrame = SimulationConfig::fixedTimeStep * 3.0;
        if (accumulator > maxFrame) {
            accumulator = maxFrame;
        }

        while (running_ && accumulator >= SimulationConfig::fixedTimeStep) {
            engine_.Step();
            accumulator -= SimulationConfig::fixedTimeStep;
        }

        engine_.UpdateVisualState();

        renderer_.Render(engine_, running_, renderer_.IsInsideButton(mouseX, mouseY));
        SDL_Delay(1);
    }

    return 0;
}
