#include "physics/Ball.h"

Ball::Ball(std::size_t idValue, Vec2 positionValue, double radiusValue)
    : id(idValue), position(positionValue), renderPosition(positionValue), spawnPosition(positionValue), targetPosition(positionValue), radius(radiusValue) {
}

CollisionObjectType Ball::GetType() const {
    return CollisionObjectType::Ball;
}

void Ball::Wake() {
    sleeping = false;
    sleepCounter = 0;
}
