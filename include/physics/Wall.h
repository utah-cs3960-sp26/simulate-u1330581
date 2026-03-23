#pragma once

#include "physics/CollisionObject.h"

enum class WallOrientation {
    Left,
    Right,
    Floor
};

class Wall : public CollisionObject {
public:
    Wall(WallOrientation orientation, double coordinate);

    /** Returns the runtime collision type for dispatch. */
    CollisionObjectType GetType() const override;

    WallOrientation orientation;
    double coordinate;
};
