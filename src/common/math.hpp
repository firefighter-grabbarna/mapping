#pragma once

struct Vec2 {
public:
    float x, y;

    Vec2() : x(0), y(0) {}
    Vec2(float x, float y) : x(x), y(y) {}

    Vec2 operator+(Vec2 rhs);
    Vec2 operator-(Vec2 rhs);
    Vec2 operator*(float rhs);
    Vec2 operator/(float rhs);
    bool operator==(Vec2 rhs);

    float mag();
    float dot(Vec2 other);
};
