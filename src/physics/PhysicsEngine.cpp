#include "physics/PhysicsEngine.h"

#include <algorithm>
#include <cmath>

namespace {
constexpr double kBallPenetrationEpsilon = 0.06;
constexpr double kWallPenetrationEpsilon = 0.02;
constexpr double kGridCellSize = SimulationConfig::ballRadius * 2.6;
}

PhysicsEngine::PhysicsEngine(Simulation simulation)
    : simulation_(std::move(simulation)) {
}

void PhysicsEngine::Step() {
    for (Ball& ball : simulation_.GetBalls()) {
        if (!ball.active && !ball.sleeping && frameCount_ >= ball.activationFrame) {
            ball.active = true;
            ball.position = ball.spawnPosition;
            ball.renderPosition = ball.spawnPosition;
            ball.velocity = ball.launchVelocity;
        }
    }

    const double dt = SimulationConfig::fixedTimeStep / static_cast<double>(SimulationConfig::substeps);
    for (int substep = 0; substep < SimulationConfig::substeps; ++substep) {
        Integrate(dt);
        for (int iteration = 0; iteration < SimulationConfig::solverIterations; ++iteration) {
            RebuildSpatialGrid();
            ResolveBallCollisions();
            ResolveWallCollisions();
        }
        UpdateSettledState();
    }
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

void PhysicsEngine::UpdateVisualState() {
    for (Ball& ball : simulation_.GetBalls()) {
        if (!ball.active && !ball.sleeping) {
            continue;
        }

        if (!ball.sleeping) {
            ball.renderPosition = ball.position;
            continue;
        }

        const Vec2 delta = ball.position - ball.renderPosition;
        if (LengthSquared(delta) <= 0.01) {
            ball.renderPosition = ball.position;
            continue;
        }

        ball.renderPosition += delta * 0.3;
    }
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

        ball.velocity.y += SimulationConfig::gravity * dt;
        ball.velocity *= SimulationConfig::linearDamping;
        ball.position += ball.velocity * dt;
        ClampVelocity(ball);
    }
}

void PhysicsEngine::RebuildSpatialGrid() {
    spatialGrid_.clear();
    const auto& balls = simulation_.GetBalls();
    for (std::size_t index = 0; index < balls.size(); ++index) {
        const Ball& ball = balls[index];
        if (!ball.active && !ball.sleeping) {
            continue;
        }

        const int cellX = static_cast<int>(std::floor(ball.position.x / kGridCellSize));
        const int cellY = static_cast<int>(std::floor(ball.position.y / kGridCellSize));
        spatialGrid_[{cellX, cellY}].push_back(index);
    }
}

void PhysicsEngine::ResolveBallCollisions() {
    auto& balls = simulation_.GetBalls();
    for (const auto& [cell, indices] : spatialGrid_) {
        for (int offsetY = -1; offsetY <= 1; ++offsetY) {
            for (int offsetX = -1; offsetX <= 1; ++offsetX) {
                const CellKey neighbor {cell.first + offsetX, cell.second + offsetY};
                if (neighbor < cell) {
                    continue;
                }

                const auto found = spatialGrid_.find(neighbor);
                if (found == spatialGrid_.end()) {
                    continue;
                }

                for (std::size_t leftIndex : indices) {
                    for (std::size_t rightIndex : found->second) {
                        if (cell == neighbor && leftIndex >= rightIndex) {
                            continue;
                        }
                        if (cell != neighbor && leftIndex == rightIndex) {
                            continue;
                        }
                        ResolveBallPair(balls[leftIndex], balls[rightIndex]);
                    }
                }
            }
        }
    }
}

void PhysicsEngine::ResolveWallCollisions() {
    auto& balls = simulation_.GetBalls();
    for (Ball& ball : balls) {
        if (!ball.active || ball.sleeping) {
            continue;
        }
        for (const Wall& wall : simulation_.GetWalls()) {
            ResolveWall(ball, wall);
        }
    }
}

void PhysicsEngine::UpdateSettledState() {
    for (Ball& ball : simulation_.GetBalls()) {
        if (!ball.active || ball.sleeping) {
            continue;
        }

        const bool timedOutToRest = frameCount_ > (ball.activationFrame + 220U);
        if (CanSettle(ball) || timedOutToRest) {
            ball.position = ball.targetPosition;
            ball.velocity = Vec2 {};
            ball.sleeping = true;
            ball.sleepCounter = SimulationConfig::sleepFrames;
        }
    }
}

bool PhysicsEngine::CanSettle(const Ball& ball) const {
    const Vec2 toTarget = ball.targetPosition - ball.position;
    const auto& bounds = simulation_.GetBounds();
    if (ball.slotRow == 0) {
        const bool touchingFloor = (bounds.bottom - (ball.position.y + ball.radius)) <= SimulationConfig::supportTolerance;
        const bool closeEnoughForFloorSnap = std::abs(ball.position.x - ball.targetPosition.x) <= (SimulationConfig::ballRadius * 6.0);
        if (touchingFloor && closeEnoughForFloorSnap) {
            return true;
        }
    }

    if (Length(toTarget) > SimulationConfig::settleDistance) {
        return false;
    }

    for (const Ball& other : simulation_.GetBalls()) {
        if (other.id == ball.id || (!other.active && !other.sleeping)) {
            continue;
        }

        const Vec2 delta = other.position - ball.targetPosition;
        const double minDistance = ball.radius + other.radius;
        if (LengthSquared(delta) < minDistance * minDistance - 1e-4 && !other.sleeping) {
            return false;
        }
    }

    if (ball.slotRow == 0) {
        if (ball.launchFromLeft) {
            if (ball.slotColumn > 0) {
                const Ball* leftNeighbor = FindBallAtSlot(ball.slotRow, ball.slotColumn - 1U);
                if (leftNeighbor != nullptr && !leftNeighbor->sleeping) {
                    const bool neighborReady = Length(leftNeighbor->targetPosition - leftNeighbor->position) <= SimulationConfig::settleDistance
                        && LengthSquared(leftNeighbor->velocity) <= SimulationConfig::sleepSpeed * SimulationConfig::sleepSpeed;
                    if (!neighborReady) {
                        return false;
                    }
                }
            }
        } else {
            const Ball* rightNeighbor = FindBallAtSlot(ball.slotRow, ball.slotColumn + 1U);
            if (rightNeighbor != nullptr && !rightNeighbor->sleeping) {
                const bool neighborReady = Length(rightNeighbor->targetPosition - rightNeighbor->position) <= SimulationConfig::settleDistance
                    && LengthSquared(rightNeighbor->velocity) <= SimulationConfig::sleepSpeed * SimulationConfig::sleepSpeed;
                if (!neighborReady) {
                    return false;
                }
            }
        }
    }

    return HasSupport(ball);
}

bool PhysicsEngine::HasSupport(const Ball& ball) const {
    if (ball.slotRow == 0) {
        return true;
    }

    if (ball.position.y >= ball.targetPosition.y - (SimulationConfig::ballRadius * 3.0)) {
        return true;
    }

    const std::size_t belowRow = ball.slotRow - 1;
    if ((ball.slotRow % 2U) == 1U) {
        const Ball* leftSupport = FindBallAtSlot(belowRow, ball.slotColumn);
        const Ball* rightSupport = FindBallAtSlot(belowRow, ball.slotColumn + 1U);
        return leftSupport != nullptr && rightSupport != nullptr
            && leftSupport->sleeping && rightSupport->sleeping;
    }

    if (ball.slotColumn == 0) {
        return false;
    }

    const Ball* leftSupport = FindBallAtSlot(belowRow, ball.slotColumn - 1U);
    const Ball* rightSupport = FindBallAtSlot(belowRow, ball.slotColumn);
    return leftSupport != nullptr && rightSupport != nullptr
        && leftSupport->sleeping && rightSupport->sleeping;
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
    if ((!a.active && !a.sleeping) || (!b.active && !b.sleeping)) {
        return;
    }

    if (!a.sleeping && !b.sleeping && a.launchFromLeft != b.launchFromLeft) {
        return;
    }

    Vec2 delta = b.position - a.position;
    const double minDistance = a.radius + b.radius;
    const double distanceSquared = LengthSquared(delta);
    if (distanceSquared >= minDistance * minDistance) {
        return;
    }

    const double distance = std::sqrt(std::max(distanceSquared, 1e-12));
    const Vec2 normal = distance > 1e-6 ? delta / distance : Vec2 {1.0, 0.0};
    const double penetration = minDistance - distance + kBallPenetrationEpsilon;

    if (a.sleeping && !b.sleeping) {
        b.position += penetration * normal;
    } else if (!a.sleeping && b.sleeping) {
        a.position -= penetration * normal;
    } else if (!a.sleeping && !b.sleeping) {
        const Vec2 correction = 0.5 * penetration * normal;
        a.position -= correction;
        b.position += correction;
    } else {
        return;
    }

    const Vec2 relativeVelocity = b.velocity - a.velocity;
    const double normalSpeed = Dot(relativeVelocity, normal);
    if (normalSpeed >= 0.0) {
        return;
    }

    if (a.sleeping && !b.sleeping) {
        b.velocity -= (1.0 + SimulationConfig::restitution) * normalSpeed * normal;
        ClampVelocity(b);
        return;
    }

    if (!a.sleeping && b.sleeping) {
        a.velocity += (1.0 + SimulationConfig::restitution) * normalSpeed * normal;
        ClampVelocity(a);
        return;
    }

    const Vec2 impulse = 0.5 * (1.0 + SimulationConfig::restitution) * normalSpeed * normal;
    a.velocity += impulse;
    b.velocity -= impulse;
    ClampVelocity(a);
    ClampVelocity(b);
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
