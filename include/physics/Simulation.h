#pragma once

#include <vector>

#include "physics/Ball.h"
#include "physics/Config.h"
#include "physics/Wall.h"

struct ContainerBounds {
    double left {0.0};
    double right {0.0};
    double top {0.0};
    double bottom {0.0};
};

class Simulation {
public:
    Simulation(std::vector<Ball> balls, std::vector<Wall> walls, ContainerBounds bounds);

    /** Returns mutable access to the simulation balls. */
    std::vector<Ball>& GetBalls();

    /** Returns read-only access to the simulation balls. */
    const std::vector<Ball>& GetBalls() const;

    /** Returns read-only access to the walls. */
    const std::vector<Wall>& GetWalls() const;

    /** Returns the inner container bounds. */
    const ContainerBounds& GetBounds() const;

private:
    std::vector<Ball> balls_;
    std::vector<Wall> walls_;
    ContainerBounds bounds_;
};
