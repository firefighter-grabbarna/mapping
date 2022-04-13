#pragma once

#include <string>
#include <vector>

class Serial {
public:
    // Connects to a serial port.
    Serial(const char *filename);
    // Move constructor.
    Serial(Serial &&serial);
    // Disconnects from the serial port.
    ~Serial();
    // Writes the specified bytes.
    void output(const char *data);
    // Reads a single line from the port, and removes the line break.
    std::string input();
    // Reads until the serial blocks.
    void readUntilBlock();
    // Sends a line and waits for the response until the first empty line.
    std::vector<std::string> query(const char *command);
private:
    // The file handle.
    int fd;
};
