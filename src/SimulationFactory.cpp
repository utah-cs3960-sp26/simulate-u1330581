#include "SimulationFactory.h"

#include <algorithm>
#include <array>
#include <cmath>
#include <string_view>
#include <vector>

namespace {
using GlyphRows = std::array<std::string_view, 7>;
constexpr std::array<double, 21> kOuterHalfWidthsTopDown {
    0.08, 0.14, 0.24, 0.34, 0.40, 0.43, 0.43,
    0.43, 0.43, 0.43, 0.43, 0.43, 0.43, 0.42,
    0.41, 0.39, 0.36, 0.32, 0.27, 0.20, 0.12
};
constexpr double kBorderThickness = 0.055;

constexpr BallColor kBlack {0, 0, 0, 255};
constexpr BallColor kGold {245, 201, 23, 255};
constexpr BallColor kRed {220, 0, 50, 255};
constexpr BallColor kBlue {0, 77, 152, 255};
constexpr BallColor kClaret {165, 0, 68, 255};
constexpr BallColor kWhite {245, 245, 245, 255};

constexpr GlyphRows kGlyphF {
    "11111",
    "10000",
    "11110",
    "10000",
    "10000",
    "10000",
    "10000"
};

constexpr GlyphRows kGlyphC {
    "01110",
    "10001",
    "10000",
    "10000",
    "10000",
    "10001",
    "01110"
};

constexpr GlyphRows kGlyphB {
    "11110",
    "10001",
    "11110",
    "10001",
    "10001",
    "10001",
    "11110"
};

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

bool IsGlyphPixel(const GlyphRows& glyph, int x, int y) {
    if (x < 0 || x >= 5 || y < 0 || y >= 7) {
        return false;
    }
    return glyph[static_cast<std::size_t>(y)][static_cast<std::size_t>(x)] == '1';
}

bool IsFcbLetter(double xNorm, double yNorm) {
    if (yNorm < 0.40 || yNorm > 0.58 || xNorm < 0.28 || xNorm > 0.72) {
        return false;
    }

    constexpr std::array<GlyphRows, 3> glyphs {kGlyphF, kGlyphC, kGlyphB};
    const double localX = (xNorm - 0.28) / 0.44;
    const double localY = (yNorm - 0.40) / 0.18;
    const int glyphIndex = std::clamp(static_cast<int>(localX * 3.0), 0, 2);
    const double glyphLocalX = (localX * 3.0) - static_cast<double>(glyphIndex);
    const int pixelX = static_cast<int>(glyphLocalX * 6.0);
    const int pixelY = static_cast<int>(localY * 7.0);
    return IsGlyphPixel(glyphs[static_cast<std::size_t>(glyphIndex)], pixelX, pixelY);
}

double ShieldHalfWidthForRow(std::size_t row, std::size_t totalRows) {
    const double topRow = static_cast<double>((totalRows - 1U) - row);
    const double sample = topRow * static_cast<double>(kOuterHalfWidthsTopDown.size() - 1U)
        / static_cast<double>(totalRows - 1U);
    const std::size_t lower = static_cast<std::size_t>(std::floor(sample));
    const std::size_t upper = std::min(lower + 1U, kOuterHalfWidthsTopDown.size() - 1U);
    const double blend = sample - static_cast<double>(lower);
    return kOuterHalfWidthsTopDown[lower] * (1.0 - blend) + kOuterHalfWidthsTopDown[upper] * blend;
}

bool IsInsideShield(std::size_t row, std::size_t totalRows, double xNorm, double inset) {
    const double centeredX = std::abs(xNorm - 0.5);
    double halfWidth = ShieldHalfWidthForRow(row, totalRows) - inset;
    if (halfWidth <= 0.0) {
        return false;
    }

    const std::size_t topRow = (totalRows - 1U) - row;
    if (topRow <= 2U) {
        const double notch = 0.08 * static_cast<double>(3U - topRow);
        if (std::abs(xNorm - 0.5) < notch) {
            return false;
        }
    }

    return centeredX <= halfWidth;
}

BallColor CrestColorFor(std::size_t row, std::size_t totalRows, double xNorm) {
    const double yNorm = static_cast<double>(row) / static_cast<double>(totalRows - 1U);
    const std::size_t topRow = (totalRows - 1U) - row;
    if (!IsInsideShield(row, totalRows, xNorm, 0.0)) {
        return kWhite;
    }
    if (!IsInsideShield(row, totalRows, xNorm, kBorderThickness)) {
        return kGold;
    }

    if (topRow <= 1U) {
        return kGold;
    }

    if (topRow <= 6U) {
        if (xNorm < 0.5) {
            const bool verticalCross = std::abs(xNorm - 0.29) <= 0.065;
            const bool horizontalCross = topRow >= 3U && topRow <= 4U;
            return (verticalCross || horizontalCross) ? kRed : kWhite;
        }

        const double stripeX = (xNorm - 0.5) / 0.5;
        const int stripeIndex = static_cast<int>(std::floor(stripeX * 6.0));
        return (stripeIndex % 2) == 0 ? kGold : kRed;
    }

    if (topRow <= 9U) {
        return IsFcbLetter(xNorm, yNorm) ? kBlack : kGold;
    }

    const double dx = xNorm - 0.5;
    const double dy = yNorm - 0.19;
    const double distanceSquared = dx * dx + dy * dy;
    if (distanceSquared <= 0.013) {
        return kGold;
    }
    if (distanceSquared <= 0.0175) {
        return kBlack;
    }

    const int stripeIndex = static_cast<int>(std::floor(xNorm * 5.0));
    return (stripeIndex % 2) == 0 ? kBlue : kClaret;
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

            const double xNorm = (x - bounds.left) / (bounds.right - bounds.left);
            ball.color = CrestColorFor(row, SimulationConfig::rows, xNorm);
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
