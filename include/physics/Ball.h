#pragma once

#include <cstddef>

#include "physics/CollisionObject.h"

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
    double flightDuration {0.0};
    double radius;
    std::size_t slotRow {0};
    std::size_t slotColumn {0};
    std::size_t activationFrame {0};
    bool launchFromLeft {true};
    bool active {false};
    bool sleeping {false};
    int sleepCounter {0};
};
