#include "SimulationFactory.h"

#include <cmath>

namespace {
std::size_t ColumnsForRow(std::size_t row) {
    return (row % 2U) == 1U ? SimulationConfig::oddRowColumns : SimulationConfig::evenRowColumns;
}

double PackedWidth() {
    return static_cast<double>(SimulationConfig::evenRowColumns) * SimulationConfig::ballRadius * 2.0;
}

std::size_t RowStart(std::size_t row) {
    std::size_t start = 0;
    for (std::size_t current = 0; current < row; ++current) {
        start += ColumnsForRow(current);
    }
    return start;
}

std::vector<Ball> CreateBalls(const ContainerBounds& bounds) {
    std::vector<Ball> balls;
    balls.reserve(SimulationConfig::evenRowColumns * ((SimulationConfig::rows + 1) / 2)
        + SimulationConfig::oddRowColumns * (SimulationConfig::rows / 2));

    const double diameter = SimulationConfig::ballRadius * 2.0;
    const double rowSpacing = diameter * std::sqrt(3.0) * 0.5;
    const double centerX = (bounds.left + bounds.right) * 0.5;
    const Vec2 leftSpawnBase {centerX - SimulationConfig::launchSpawnOffsetX, bounds.top - SimulationConfig::launchSpawnOffsetY};
    const Vec2 rightSpawnBase {centerX + SimulationConfig::launchSpawnOffsetX, bounds.top - SimulationConfig::launchSpawnOffsetY};
    std::size_t id = 0;
    for (std::size_t row = 0; row < SimulationConfig::rows; ++row) {
        const bool oddRow = (row % 2U) == 1U;
        const std::size_t columns = oddRow ? SimulationConfig::oddRowColumns : SimulationConfig::evenRowColumns;
        const double rowOffset = oddRow ? SimulationConfig::ballRadius * 2.0 : SimulationConfig::ballRadius;
        const double y = bounds.bottom - SimulationConfig::ballRadius - static_cast<double>(row) * rowSpacing;
        for (std::size_t column = 0; column < columns; ++column) {
            const double x = bounds.left + rowOffset + static_cast<double>(column) * diameter;
            balls.emplace_back(id++, Vec2 {x, y}, SimulationConfig::ballRadius);
            Ball& ball = balls.back();
            ball.targetPosition = Vec2 {x, y};
            ball.slotRow = row;
            ball.slotColumn = column;
            ball.launchFromLeft = x <= centerX;
        }
    }

    std::size_t leftLaneIndex = 0;
    std::size_t rightLaneIndex = 0;
    for (std::size_t row = 0; row < SimulationConfig::rows; ++row) {
        const std::size_t columns = ColumnsForRow(row);
        const std::size_t start = RowStart(row);
        for (std::size_t column = 0; column < columns; ++column) {
            Ball& ball = balls[start + column];
            if (!ball.launchFromLeft) {
                continue;
            }
            const Vec2 spawn {
                leftSpawnBase.x,
                leftSpawnBase.y - static_cast<double>(leftLaneIndex) * (SimulationConfig::ballRadius * 0.35)
            };
            ball.spawnPosition = spawn;
            ball.position = spawn;
            ball.activationFrame = leftLaneIndex * SimulationConfig::releaseIntervalFrames;
            const double flightTime = SimulationConfig::launchFlightTime
                + (std::abs(ball.targetPosition.x - spawn.x) / 8000.0)
                + static_cast<double>(row) * 0.0008;
            ball.flightDuration = flightTime;
            ball.launchVelocity.x = (ball.targetPosition.x - spawn.x) / flightTime;
            ball.launchVelocity.y = (ball.targetPosition.y - spawn.y
                - 0.5 * SimulationConfig::gravity * flightTime * flightTime) / flightTime;
            ++leftLaneIndex;
        }

        for (std::size_t offset = 0; offset < columns; ++offset) {
            const std::size_t column = columns - 1U - offset;
            Ball& ball = balls[start + column];
            if (ball.launchFromLeft) {
                continue;
            }
            const Vec2 spawn {
                rightSpawnBase.x,
                rightSpawnBase.y - static_cast<double>(rightLaneIndex) * (SimulationConfig::ballRadius * 0.35)
            };
            ball.spawnPosition = spawn;
            ball.position = spawn;
            ball.activationFrame = rightLaneIndex * SimulationConfig::releaseIntervalFrames;
            const double flightTime = SimulationConfig::launchFlightTime
                + (std::abs(ball.targetPosition.x - spawn.x) / 8000.0)
                + static_cast<double>(row) * 0.0008;
            ball.flightDuration = flightTime;
            ball.launchVelocity.x = (ball.targetPosition.x - spawn.x) / flightTime;
            ball.launchVelocity.y = (ball.targetPosition.y - spawn.y
                - 0.5 * SimulationConfig::gravity * flightTime * flightTime) / flightTime;
            ++rightLaneIndex;
        }
    }

    return balls;
}

std::vector<Wall> CreateWalls(const ContainerBounds& bounds) {
    return {
        Wall {WallOrientation::Left, bounds.left},
        Wall {WallOrientation::Right, bounds.right},
        Wall {WallOrientation::Floor, bounds.bottom}
    };
}
}

namespace SimulationFactory {
Simulation CreateSimulation() {
    const double halfSize = SimulationConfig::containerInnerSize * 0.5;
    const double centerX = SimulationConfig::windowWidth * 0.5;
    const double centerY = SimulationConfig::windowHeight * 0.57;
    const double left = centerX - halfSize;
    const ContainerBounds bounds {
        left,
        left + PackedWidth(),
        centerY - halfSize,
        centerY + halfSize
    };

    return Simulation {CreateBalls(bounds), CreateWalls(bounds), bounds};
}
}
