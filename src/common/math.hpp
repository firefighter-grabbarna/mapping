#pragma once

#include <optional>

struct Vec2;
struct Point;

// A vector.
struct Vec2 {
    float x, y;

    Vec2();
    Vec2(float x, float y) : x(x), y(y) {}
    Vec2(Point point);

    // Magnitude of the vector.
    float mag() const;
    // Dot product.
    float dot(Vec2 other) const;
    // Cross product.
    float cross(Vec2 other) const;

    Vec2 operator+(Vec2 rhs) const;
    Vec2 operator-(Vec2 rhs) const;
    Point operator+(Point rhs) const;
    Point operator-(Point rhs) const;
    Vec2 operator*(float rhs) const;
    Vec2 operator/(float rhs) const;
};

// A point.
struct Point {
    float x, y;

    Point();
    Point(float x, float y) : x(x), y(y) {}
    Point(Vec2 vec);

    Point operator+(Vec2 rhs) const;
    Point operator-(Vec2 rhs) const;
    Vec2 operator-(Point rhs) const;
};

// A line segment.
struct Line {
    Point p1, p2;

    // The point in the line closest to the target.
    Point pointClosestTo(Point target) const;
};

// A ray with infinite length.
struct Ray {
    Point origin;
    Vec2 direction;

    Ray(Point origin, Vec2 direction) : origin(origin), direction(direction) {}

    std::optional<Point> castOnto(Line line) const;
};
