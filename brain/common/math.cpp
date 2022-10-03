#include "math.hpp"

#include <cmath>
#include <optional>
#include <iostream>

float Rotation::sin() const {
    return std::sin(this->radians);
}
float Rotation::cos() const {
    return std::cos(this->radians);
}

Rotation Rotation::operator-() const {
    return Rotation(-this->radians);
}
Rotation Rotation::operator+(Rotation rhs) const {
    return Rotation(this->radians + rhs.radians);
}
Rotation Rotation::operator-(Rotation rhs) const {
    return Rotation(this->radians - rhs.radians);
}

Point Vec2::point() const {
    return Point(this->x, this->y);
}

float Vec2::mag() const {
    return hypot(this->x, this->y);
}
float Vec2::magSq() const {
    return this->x * this->x + this->y * this->y;
}
float Vec2::dot(Vec2 other) const {
    return this->x * other.x + this->y * other.y;
}
float Vec2::cross(Vec2 other) const {
    return this->x * other.y - this->y * other.x;
}
Vec2 Vec2::rotate(Rotation rotation) const {
    return Vec2(
        this->x * rotation.cos() - this->y * rotation.sin(),
        this->x * rotation.sin() + this->y * rotation.cos()
    );
}
Vec2 Vec2::normalize() const {
    if (this->mag() > 0.0001) {
        return *this / this->mag();
    } else {
        return *this;
    }
}

Vec2 Vec2::operator-() const {
    return Vec2(-this->x, -this->y);
}
Vec2 Vec2::operator+(Vec2 rhs) const {
    return Vec2(this->x + rhs.x, this->y + rhs.y);
}
Vec2 Vec2::operator-(Vec2 rhs) const {
    return Vec2(this->x - rhs.x, this->y - rhs.y);
}
Point Vec2::operator+(Point rhs) const {
    return Point(this->x + rhs.x, this->y + rhs.y);
}
Point Vec2::operator-(Point rhs) const {
    return Point(this->x - rhs.x, this->y - rhs.y);
}
Vec2 Vec2::operator*(float rhs) const {
    return Vec2(this->x * rhs, this->y * rhs);
}
Vec2 Vec2::operator/(float rhs) const {
    return Vec2(this->x / rhs, this->y / rhs);
}

Vec2 Point::vec2() const {
    return Vec2(this->x, this->y);
}

Point Point::operator+(Vec2 rhs) const {
    return Point(this->x + rhs.x, this->y + rhs.y);
}
Point Point::operator-(Vec2 rhs) const {
    return Point(this->x - rhs.x, this->y - rhs.y);
}
Vec2 Point::operator-(Point rhs) const {
    return Vec2(this->x - rhs.x, this->y - rhs.y);
}

Transform Transform::rotate(Rotation rotation) const {
    return Transform(
        this->rotation + rotation,
        this->offset.rotate(rotation)
    );
}
Transform Transform::translate(Vec2 offset) const {
    return Transform(
        this->rotation,
        this->offset + offset
    );
}

Transform Transform::inverse() const {
    return Transform(
        -this->rotation,
        -this->offset.rotate(-this->rotation)
    );
}
Vec2 Transform::applyTo(Vec2 vec) const {
    return vec.rotate(this->rotation);
}
Point Transform::applyTo(Point point) const {
    return (point.vec2().rotate(this->rotation)).point() + this->offset;
}
Ray Transform::applyTo(Ray ray) const {
    return Ray(this->applyTo(ray.origin), this->applyTo(ray.direction));
}

Point Line::pointClosestTo(Point target) const {
    Vec2 lv = this->p2 - this->p1;
    Vec2 pv = target - this->p1;

    float fraction = lv.dot(pv) / lv.dot(lv);

    if (fraction > 1) fraction = 1;
    if (fraction < 0) fraction = 0;

    return this->p1 + lv * fraction;
}

std::optional<Point> Ray::castOnto(Line line) const {
    // to do: line thickness

    Point p1 = this->origin;
    Point p2 = line.p1;
    Vec2 v1 = this->direction;
    Vec2 v2 = line.p2 - line.p1;

    if (v1.cross(v2) != 0) {
        float t1 = (p2 - p1).cross(v2) / v1.cross(v2);
        float t2 = (p2 - p1).cross(v1) / v1.cross(v2);

        if (t1 >= 0 && t2 >= 0 && t2 <= 1) {
            return this->origin + this->direction * t1;
        }
    }

    return {};
}
