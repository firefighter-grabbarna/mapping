#include "lidar.hpp"

#include <algorithm>
#include <cstring>
#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>

#include <fcntl.h>
#include <unistd.h>

#include "util.hpp"


Lidar::Lidar(const char *filename) {
    this->fd = open(filename, O_RDWR);
    if (this->fd == -1) {
        std::perror("Opening serial failed");
        std::exit(1);
    }

    // Clear the serial communication
    this->output("\n"); //< Ensure there is no partially sent command
    while (true) {
        // Wait for 100ms to ensure that the lidar has time to send data
        usleep(100'000);
        char buf[256];
        ssize_t count = read(this->fd, buf, sizeof(buf));
        if (count == -1) {
            std::perror("Reading from serial failed");
            std::exit(1);
        } else if (count == 0) {
            // Stop if there is no more data
            break;
        }
    }

    // Initialize the protocol
    auto initRes = this->query("SCIP2.0");
    assert(initRes.size() == 2, "protocol violation (1a)");
    assert(initRes[0] == "SCIP2.0", "protocol violation (1b)");
    //assert(initRes[1] == "0", "protocol violation (1c)");

    // Request parameters
    auto paramRes = this->query("PP");
    assert(paramRes.size() >= 2, "protocol violation (2a)");
    assert(paramRes[0] == "PP", "protocol violation (2b)");
    assert(paramRes[1] == "00P", "protocol violation (2c)");

    // Read all parameters
    for (size_t i = 2; i < paramRes.size(); i++) {
        auto& line = paramRes[i];

        size_t idx_1 = line.find(':');
        size_t idx_2 = line.find(';');
        assert(idx_1 < line.size(), "protocol violation (2d)");
        assert(idx_2 < line.size(), "protocol violation (2e)");
        assert(idx_1 < idx_2, "protocol violation (2f)");

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
    
    auto laserRes = this->query("BM");
    assert(laserRes.size() == 2, "protocol violation (3a)");
    assert(laserRes[0] == "BM", "protocol violation (3b)");
    assert((laserRes[1] == "00P") || (laserRes[1] == "02R"), "protocol violation (3c)");
}

Lidar::~Lidar() {
    if (this->fd != -1) {
        close(this->fd);
    }
}

std::vector<int> Lidar::scan() {
    char query[16];
    snprintf(query, sizeof(query), "GD%04d%04d01", this->amin, this->amax);
    auto scanRes = this->query(query);

    assert(scanRes.size() > 3, "protocol violation (4a)");
    assert(scanRes[0] == query, "protocol violation (4b)");
    assert(scanRes[1] == "00P", "protocol violation (4c)");

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

std::vector<std::string> Lidar::query(const char *query) {
    //std::cout << "W: " << query << std::endl;

    this->output(query);
    this->output("\n");

    std::vector<std::string> lines;
    while (true) {
        std::string line = this->input();
        if (line.empty()) break;
        //std::cout << "R: " << line << std::endl;
        lines.push_back(line);
    }

    return lines;
}

void Lidar::output(const char *data) {
    size_t len = std::strlen(data);
    while (len > 0) {
        ssize_t written = write(this->fd, data, len);
        if (written == -1) {
            std::perror("Writing to serial failed");
            std::exit(1);
        }
        data += written;
        len -= written;
    }
}

std::string Lidar::input() {
    std::string buf;
    while (true) {
        char chr;
        ssize_t count = read(this->fd, &chr, (size_t) 1);
        if (count == -1) {
            std::perror("Reading from serial failed");
            std::exit(1);
        } else if (count == 0) {
            continue;
        }
        if (chr == '\n') break;
        buf.push_back(chr);
    }
    return buf;
}
