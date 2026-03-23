#include "physics/Wall.h"

Wall::Wall(WallOrientation orientationValue, double coordinateValue)
    : orientation(orientationValue), coordinate(coordinateValue) {
}

CollisionObjectType Wall::GetType() const {
    return CollisionObjectType::Wall;
}
