#include "math.hpp"

#include <cmath>

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
