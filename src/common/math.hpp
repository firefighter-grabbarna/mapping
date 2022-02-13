#pragma once

#include <optional>

struct Rotation;
struct Vec2;
struct Point;
struct Line;
struct Ray;

// A counter clockwise rotation, in radians.
struct Rotation {
    float radians;

    Rotation();
    Rotation(float radians) : radians(radians) {}

    float sin() const;
    float cos() const;

    Rotation operator-() const;
    Rotation operator+(Rotation rhs) const;
    Rotation operator-(Rotation rhs) const;
};

// A vector.
struct Vec2 {
    float x, y;

    Vec2(): x(0), y(0) {}
    Vec2(float x, float y): x(x), y(y) {}
    Vec2(Point point);

    // Magnitude of the vector.
    float mag() const;
    // Squared magnitude of the vector.
    float magSq() const;
    // Dot product.
    float dot(Vec2 other) const;
    // Cross product.
    float cross(Vec2 other) const;

    // Rotates the vector.
    Vec2 rotate(Rotation rotation) const;

    Vec2 operator-() const;
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

    Point(): x(0), y(0) {}
    Point(float x, float y) : x(x), y(y) {}
    Point(Vec2 vec);

    Point operator+(Vec2 rhs) const;
    Point operator-(Vec2 rhs) const;
    Vec2 operator-(Point rhs) const;
};

// A rigid transformation.
struct Transform {
    Rotation rotation;
    Vec2 offset;

    Transform(): rotation(0), offset(0, 0) {}
    Transform(Rotation rotation, Vec2 offset): rotation(rotation), offset(offset) {}

    Transform inverse() const;
    Vec2 applyTo(Vec2 point) const;
    Point applyTo(Point point) const;
    Ray applyTo(Ray ray) const;
};

// A line segment.
struct Line {
    Point p1, p2;

    Line(): p1(), p2() {}
    Line(Point p1, Point p2): p1(p1), p2(p2) {}

    // The point in the line closest to the target.
    Point pointClosestTo(Point target) const;
};

// A ray with infinite length.
struct Ray {
    Point origin;
    Vec2 direction;

    Ray(Point origin, Vec2 direction): origin(origin), direction(direction) {}
    Ray(Point origin, Rotation angle): origin(origin), direction(-angle.sin(), angle.cos()) {}

    std::optional<Point> castOnto(Line line) const;
};
