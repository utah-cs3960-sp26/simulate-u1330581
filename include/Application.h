#pragma once

#include "Renderer.h"
#include "physics/PhysicsEngine.h"

class Application {
public:
    Application();

    /** Runs the SDL event loop until the window closes. */
    int Run();

private:
    Renderer renderer_;
    PhysicsEngine engine_;
    bool running_ {false};
    bool quit_ {false};
};
