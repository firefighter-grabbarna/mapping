#include "math.hpp"

#include <cmath>
#include <optional>
#include <iostream>

Vec2::Vec2() {
    this->x = 0;
    this->y = 0;
}
Vec2::Vec2(Point point) {
    this->x = point.x;
    this->y = point.y;
}

float Vec2::mag() const {
    return hypot(this->x, this->y);
}

float Vec2::dot(Vec2 other) const {
    return this->x * other.x + this->y * other.y;
}

float Vec2::cross(Vec2 other) const {
    return this->x * other.y - this->y * other.x;
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

Point::Point() {
    this->x = 0;
    this->y = 0;
}
Point::Point(Vec2 vec) {
    this->x = vec.x;
    this->y = vec.y;
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
