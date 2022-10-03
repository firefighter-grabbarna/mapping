#pragma once

#include <vector>

#include "serial.hpp"

class Lidar {
public:
    // Connects to the lidar.
    Lidar(Serial &&serial);
    // Performs a scan. Returns the distances of all angles in counter-clockwise
    // order starting behind the lidar.
    std::vector<int> scan();
private:
    // The serial interface.
    Serial serial;
    // The parameters.
    unsigned dmin, dmax, ares, amin, amax, afrt, rpm;
};
