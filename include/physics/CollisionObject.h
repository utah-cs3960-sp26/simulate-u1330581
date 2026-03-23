#pragma once

#include "physics/Vec2.h"

enum class CollisionObjectType {
    Ball,
    Wall
};

class CollisionObject {
public:
    virtual ~CollisionObject() = default;

    /** Returns the runtime collision type for dispatch. */
    virtual CollisionObjectType GetType() const = 0;
};
