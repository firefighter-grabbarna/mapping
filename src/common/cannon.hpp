#pragma once

#include "serial.hpp"

class Cannon {
public:
    // Connects to the cannon.
    Cannon(Serial &&serial);
    // Waits for the button to be pressed.
    void waitForButton();
    // Gets the angle to the fire.
    int getFireAngle();
    // Aims and shoots.
    void aimAndShoot();
private:
    // The serial interface.
    Serial serial;
};
