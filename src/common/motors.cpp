#include "motors.hpp"

#include <algorithm>
#include <cmath>
#include <iostream>

#include "serial.hpp"
#include "util.hpp"
#include "math.hpp"

Motors::Motors(Serial &&serial) : serial(std::move(serial)) {}

void Motors::setSpeed(Vec2 directional, float rotational) {
    float forward = directional.y;
    float leftward = -directional.x;

    float abs = fabs(forward) + fabs(leftward) + fabs(rotational) + 0.001;
    float scale = std::min((float) 1.0, 1 / abs) * 250.0;

    this->setSpeed(
        (int) (forward * scale),
        (int) (leftward * scale),
        (int) (rotational * scale)
    );
}

void Motors::setSpeed(int forward, int leftward, int rotational) {
    if (abs(forward) + abs(leftward) + abs(rotational) > 255) panic("Speed too large");

    char query[32];
    snprintf(query, sizeof(query), "%d %d %d\n", forward, leftward, rotational);
    this->serial.output(query);
}
