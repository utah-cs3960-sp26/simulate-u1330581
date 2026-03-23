#include "physics/Simulation.h"

Simulation::Simulation(std::vector<Ball> balls, std::vector<Wall> walls, ContainerBounds bounds)
    : balls_(std::move(balls)), walls_(std::move(walls)), bounds_(bounds) {
}

std::vector<Ball>& Simulation::GetBalls() {
    return balls_;
}

const std::vector<Ball>& Simulation::GetBalls() const {
    return balls_;
}

const std::vector<Wall>& Simulation::GetWalls() const {
    return walls_;
}

const ContainerBounds& Simulation::GetBounds() const {
    return bounds_;
}
