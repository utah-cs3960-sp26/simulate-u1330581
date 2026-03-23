#include "physics/PhysicsEngine.h"

#include <algorithm>
#include <cmath>

namespace {
constexpr double kWallPenetrationEpsilon = 0.02;
}

PhysicsEngine::PhysicsEngine(Simulation simulation)
    : simulation_(std::move(simulation)) {
}

void PhysicsEngine::Step() {
    for (Ball& ball : simulation_.GetBalls()) {
        if (!ball.active && !ball.sleeping && frameCount_ >= ball.activationFrame) {
            ball.active = true;
            ball.position = ball.spawnPosition;
            ball.velocity = ball.launchVelocity;
        }
    }

    Integrate(SimulationConfig::fixedTimeStep);
    UpdateSettledState();
    ++frameCount_;
}

Simulation& PhysicsEngine::GetSimulation() {
    return simulation_;
}

const Simulation& PhysicsEngine::GetSimulation() const {
    return simulation_;
}

bool PhysicsEngine::IsSettled() const {
    const auto& balls = simulation_.GetBalls();
    return std::all_of(balls.begin(), balls.end(), [](const Ball& ball) {
        return ball.sleeping;
    });
}

std::uint64_t PhysicsEngine::GetFrameCount() const {
    return frameCount_;
}

std::size_t PhysicsEngine::CellKeyHasher::operator()(const CellKey& key) const noexcept {
    const std::size_t left = static_cast<std::size_t>(static_cast<std::uint32_t>(key.first));
    const std::size_t right = static_cast<std::size_t>(static_cast<std::uint32_t>(key.second));
    return (left << 32U) ^ right;
}

void PhysicsEngine::Integrate(double dt) {
    for (Ball& ball : simulation_.GetBalls()) {
        if (!ball.active || ball.sleeping) {
            continue;
        }

        const double elapsed = (static_cast<double>(frameCount_ - ball.activationFrame) + 1.0) * dt;
        const double clampedElapsed = std::min(elapsed, ball.flightDuration);
        ball.position = ball.spawnPosition
            + ball.launchVelocity * clampedElapsed
            + Vec2 {0.0, 0.5 * SimulationConfig::gravity * clampedElapsed * clampedElapsed};
        ball.velocity = ball.launchVelocity + Vec2 {0.0, SimulationConfig::gravity * clampedElapsed};
        ClampVelocity(ball);
    }
}

void PhysicsEngine::RebuildSpatialGrid() {
}

void PhysicsEngine::ResolveBallCollisions() {
}

void PhysicsEngine::ResolveWallCollisions() {
}

void PhysicsEngine::UpdateSettledState() {
    for (Ball& ball : simulation_.GetBalls()) {
        if (!ball.active || ball.sleeping) {
            continue;
        }

        if (CanSettle(ball)) {
            ball.position = ball.targetPosition;
            ball.velocity = Vec2 {};
            ball.sleeping = true;
            ball.sleepCounter = SimulationConfig::sleepFrames;
        }
    }
}

bool PhysicsEngine::CanSettle(const Ball& ball) const {
    const Vec2 toTarget = ball.targetPosition - ball.position;
    if (Length(toTarget) > SimulationConfig::settleDistance) {
        return false;
    }

    if (ball.velocity.y < -SimulationConfig::sleepSpeed) {
        return false;
    }

    return true;
}

const Ball* PhysicsEngine::FindBallAtSlot(std::size_t row, std::size_t column) const {
    for (const Ball& ball : simulation_.GetBalls()) {
        if (ball.slotRow == row && ball.slotColumn == column) {
            return &ball;
        }
    }
    return nullptr;
}

Ball* PhysicsEngine::FindBallAtSlot(std::size_t row, std::size_t column) {
    for (Ball& ball : simulation_.GetBalls()) {
        if (ball.slotRow == row && ball.slotColumn == column) {
            return &ball;
        }
    }
    return nullptr;
}

std::size_t PhysicsEngine::ColumnsForRow(std::size_t row) {
    return (row % 2U) == 0U ? SimulationConfig::evenRowColumns : SimulationConfig::oddRowColumns;
}

void PhysicsEngine::ResolveBallPair(Ball& a, Ball& b) {
    (void)a;
    (void)b;
}

void PhysicsEngine::ResolveWall(Ball& ball, const Wall& wall) {
    switch (wall.orientation) {
    case WallOrientation::Left:
        if (ball.position.x - ball.radius < wall.coordinate) {
            ball.position.x = wall.coordinate + ball.radius + kWallPenetrationEpsilon;
            if (ball.velocity.x < 0.0) {
                ball.velocity.x = -ball.velocity.x * SimulationConfig::restitution;
            }
            ball.Wake();
        }
        break;
    case WallOrientation::Right:
        if (ball.position.x + ball.radius > wall.coordinate) {
            ball.position.x = wall.coordinate - ball.radius - kWallPenetrationEpsilon;
            if (ball.velocity.x > 0.0) {
                ball.velocity.x = -ball.velocity.x * SimulationConfig::restitution;
            }
            ball.Wake();
        }
        break;
    case WallOrientation::Floor:
        if (ball.position.y + ball.radius > wall.coordinate) {
            ball.position.y = wall.coordinate - ball.radius - kWallPenetrationEpsilon;
            if (ball.velocity.y > 0.0) {
                ball.velocity.y = -ball.velocity.y * SimulationConfig::restitution;
            }
            if (std::abs(ball.velocity.y) < SimulationConfig::sleepSpeed) {
                ball.velocity.y = 0.0;
            }
            ball.Wake();
        }
        break;
    }
}

void PhysicsEngine::ClampVelocity(Ball& ball) const {
    const double speedSquared = LengthSquared(ball.velocity);
    const double maxSpeedSquared = SimulationConfig::maxSpeed * SimulationConfig::maxSpeed;
    if (speedSquared <= maxSpeedSquared) {
        return;
    }
    ball.velocity = Normalize(ball.velocity) * SimulationConfig::maxSpeed;
}
