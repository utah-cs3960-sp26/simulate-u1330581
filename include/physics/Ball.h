#pragma once

#include <cstddef>
#include <cstdint>
#include <limits>

#include "physics/CollisionObject.h"

struct BallColor {
    std::uint8_t r {255};
    std::uint8_t g {255};
    std::uint8_t b {255};
    std::uint8_t a {255};
};

class Ball : public CollisionObject {
public:
    Ball(std::size_t id, Vec2 position, double radius);

    /** Returns the runtime collision type for dispatch. */
    CollisionObjectType GetType() const override;

    /** Wakes the ball so the solver updates it again. */
    void Wake();

    std::size_t id;
    Vec2 position;
    Vec2 renderPosition;
    Vec2 velocity;
    Vec2 spawnPosition;
    Vec2 targetPosition;
    Vec2 launchVelocity;
    BallColor color {};
    double flightDuration {0.0};
    double radius;
    std::size_t slotRow {0};
    std::size_t slotColumn {0};
    std::size_t activationFrame {0};
    std::size_t supportLeftIndex {std::numeric_limits<std::size_t>::max()};
    std::size_t supportRightIndex {std::numeric_limits<std::size_t>::max()};
    bool launchFromLeft {true};
    bool active {false};
    bool sleeping {false};
    int sleepCounter {0};
};
