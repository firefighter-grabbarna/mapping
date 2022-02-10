#include "math.hpp"

#include <cmath>

Vec2 Vec2::operator+(Vec2 rhs) {
    return Vec2(this->x + rhs.x, this->y + rhs.y);
}
Vec2 Vec2::operator-(Vec2 rhs) {
    return Vec2(this->x - rhs.x, this->y - rhs.y);
}
Vec2 Vec2::operator*(float rhs) {
    return Vec2(this->x * rhs, this->y * rhs);
}
Vec2 Vec2::operator/(float rhs) {
    return Vec2(this->x / rhs, this->y / rhs);
}
bool Vec2::operator==(Vec2 rhs) {
    return this->x == rhs.x && this->y == rhs.y;
}

float Vec2::mag() {
    return hypot(this->x, this->y);
}
float Vec2::dot(Vec2 rhs) {
    return this->x * rhs.x + this->y * rhs.y;
}
