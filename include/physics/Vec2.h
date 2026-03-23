#pragma once

#include <cmath>

struct Vec2 {
    double x {0.0};
    double y {0.0};

    Vec2& operator+=(const Vec2& other) {
        x += other.x;
        y += other.y;
        return *this;
    }

    Vec2& operator-=(const Vec2& other) {
        x -= other.x;
        y -= other.y;
        return *this;
    }

    Vec2& operator*=(double scalar) {
        x *= scalar;
        y *= scalar;
        return *this;
    }
};

inline Vec2 operator+(Vec2 lhs, const Vec2& rhs) {
    lhs += rhs;
    return lhs;
}

inline Vec2 operator-(Vec2 lhs, const Vec2& rhs) {
    lhs -= rhs;
    return lhs;
}

inline Vec2 operator*(Vec2 value, double scalar) {
    value *= scalar;
    return value;
}

inline Vec2 operator*(double scalar, Vec2 value) {
    value *= scalar;
    return value;
}

inline Vec2 operator/(Vec2 value, double scalar) {
    value.x /= scalar;
    value.y /= scalar;
    return value;
}

inline double Dot(const Vec2& lhs, const Vec2& rhs) {
    return lhs.x * rhs.x + lhs.y * rhs.y;
}

inline double LengthSquared(const Vec2& value) {
    return Dot(value, value);
}

inline double Length(const Vec2& value) {
    return std::sqrt(LengthSquared(value));
}

inline Vec2 Normalize(const Vec2& value) {
    const double len = Length(value);
    if (len <= 1e-9) {
        return Vec2 {1.0, 0.0};
    }
    return value / len;
}
