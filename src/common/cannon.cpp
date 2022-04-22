#include "cannon.hpp"

#include "serial.hpp"

Cannon::Cannon(Serial &&serial) : serial(std::move(serial)) {}

// Waits for the button to be pressed.
void Cannon::waitForButton() {
    this->serial.query("W");
}

// Gets the angle to the fire.
int Cannon::getFireAngle() {
    auto lines = this->serial.query("A");
    return atoi(lines[0].c_str());
}

// Aims and shoots.
void Cannon::aimAndShoot() {
    this->serial.query("S");
}
