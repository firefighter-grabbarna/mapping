#include "lidar.hpp"

#include <algorithm>
#include <iostream>
#include <string>
#include <vector>

#include "serial.hpp"
#include "util.hpp"

static void violation(std::string got, std::string number) {
    char buf[512];
    snprintf(buf, sizeof(buf), "protocol violation (%s), got '%s'", number.c_str(), got.c_str());
    panic(buf);
}
static void ensureStr(std::string got, std::string expected, std::string number) {
    if (got != expected) {
        violation(got, number);
    }
}
static void ensure(bool condition, std::string number) {
    if (!condition) {
        char buf[512];
        snprintf(buf, sizeof(buf), "protocol violation (%s)", number.c_str());
        panic(buf);
    }
}

Lidar::Lidar(Serial &&serial) : serial(std::move(serial)) {

    // // Initialize the protocol
    // auto initRes = this->serial.query("SCIP2.0");
    // ensure(initRes.size() == 2, "1a");
    // ensureStr(initRes[0], "SCIP2.0", "1b");
    // //ensureStr(initRes[1], "0", "1c");

    // Request parameters
    auto paramRes = this->serial.query("PP");
    ensure(paramRes.size() >= 2, "2a");
    ensureStr(paramRes[0], "PP", "2b");
    ensureStr(paramRes[1], "00P", "2c");

    // Read all parameters
    for (size_t i = 2; i < paramRes.size(); i++) {
        auto& line = paramRes[i];

        size_t idx_1 = line.find(':');
        size_t idx_2 = line.find(';');
        ensure(idx_1 < line.size(), "2d");
        ensure(idx_2 < line.size(), "2e");
        ensure(idx_1 < idx_2, "2f");

        std::string key = line.substr(0, idx_1);
        std::string value = line.substr(idx_1 + 1, idx_2 - idx_1 - 1);

        if (key == "DMIN") this->dmin = atoi(value.c_str());
        if (key == "DMAX") this->dmax = atoi(value.c_str());
        if (key == "ARES") this->ares = atoi(value.c_str());
        if (key == "AMIN") this->amin = atoi(value.c_str());
        if (key == "AMAX") this->amax = atoi(value.c_str());
        if (key == "AFRT") this->afrt = atoi(value.c_str());
        if (key == "SCAN") this->rpm = atoi(value.c_str());
    }
    std::cout << "D: " << this->dmin << ".." << this->dmax << "; A: " << this->ares
        << ", " << this->amin << ".." << this->afrt << ".." << this->amax
        << "; R: " << this->rpm << std::endl;

    auto laserRes = this->serial.query("BM");
    ensure(laserRes.size() == 2, "3a");
    ensureStr(laserRes[0], "BM", "3b");
    if ((laserRes[1] != "00P") && (laserRes[1] != "02R")) {
        violation(laserRes[1], "3c");
    }
}

std::vector<int> Lidar::scan() {
    char query[16];
    snprintf(query, sizeof(query), "GD%04d%04d01", this->amin, this->amax);
    auto scanRes = this->serial.query(query);

    ensure(scanRes.size() > 3, "4a");
    ensureStr(scanRes[0], query, "4b");
    ensureStr(scanRes[1], "00P", "4c");

    // Concatenate the encoded data
    std::string data;
    for (size_t i = 3; i < scanRes.size(); i++) {
        auto& line = scanRes[i];
        data += line.substr(0, line.size() - 1);
    }

    // Decode the data
    std::vector<int> values;    
    for (size_t i = 0; i + 2 < data.size(); i += 3) {
        unsigned value = 0;
        for (size_t j = i; j < i + 3; j++) {
            value = value * 64 + (unsigned) data[j] - '0';
        }

        if ((value < this->dmin) || (value > this->dmax)) value = -1;
        values.push_back(value);
    }

    // Extend to the full angular resolution
    while (values.size() < (size_t) this->ares)
        values.push_back(-1);

    // Rotate so that the angles are in the correct position
    int right_rotate = this->amin - this->afrt + this->ares / 2;
    right_rotate = (right_rotate + this->ares) % this->ares;
    std::rotate(values.begin(), values.end() - right_rotate, values.end());

    return values;
}
