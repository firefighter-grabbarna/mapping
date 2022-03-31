#pragma once

#include <string>
#include <vector>

class Serial {
public:
    // Connects to a serial port.
    Serial(const char *filename);
    // Disconnects from the serial port.
    ~Serial();
    // Writes the specified bytes.
    void output(const char *data);
    // Reads a single line from the port, and removes the line break.
    std::string input();
    // Reads until the serial blocks.
    void readUntilBlock();
private:
    // The file handle.
    int fd;
};
