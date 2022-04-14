#pragma once

#include "serial.hpp"
#include "math.hpp"

class Motors {
public:
    // Connects to the motors.
    Motors(Serial &&serial);
    // Sets the speed of the motors, where 1.0 is the maximum.
    void setSpeed(Vec2 directional, float rotational);
private:
    // The serial interface.
    Serial serial;
    // Sets the speed of the motors, where 255 is the maximum.
    // The sum of the absolute values of the velocities must not exceed 255.
    void setSpeed(int forward, int rightward, int rotational);
};
