#include "serial.hpp"

#include <cstring>
#include <iostream>
#include <string>

#include <fcntl.h>
#include <unistd.h>
#include <termios.h>

#include "util.hpp"

Serial::Serial(const char *filename) {
    this->fd = open(filename, O_RDWR | O_NOCTTY | O_SYNC);
    if (this->fd == -1) {
        std::perror("Opening serial failed");
        std::exit(1);
    }

    struct termios term;
	tcgetattr(fd, &term);

	term.c_cflag |= CS8;
	term.c_oflag &= ~(ONLCR | ONOCR | OCRNL | OLCUC);
	term.c_lflag &= ~(ECHO | ICANON);
	term.c_iflag &= ~(INPCK | ISTRIP);
    term.c_iflag |= IGNCR;

	term.c_cc[VMIN] = 0;
	term.c_cc[VTIME] = 0;

	tcsetattr(fd, TCSAFLUSH, &term);

    usleep(500'000);

    // Clear the serial communication
    this->output("\n"); // Ensure there is no partially sent command
    this->readUntilBlock(); // Wait for the response from the partial command
}

Serial::Serial(Serial &&other) {
    this->fd = other.fd;
    other.fd = -1;
}

Serial::~Serial() {
    if (this->fd != -1) {
        close(this->fd);
    }
}

void Serial::output(const char *data) {
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

std::string Serial::input() {
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

void Serial::readUntilBlock() {
    while (true) {
        // Wait for 100ms to ensure that data has time to be received.
        usleep(100'000);
        char buf[512];
        ssize_t count = read(this->fd, buf, sizeof(buf));
        if (count == -1) {
            std::perror("Reading from serial failed");
            std::exit(1);
        } else if (count == 0) {
            // Stop if there is no more data
            break;
        }
    }
}

std::vector<std::string> Serial::query(const char *query) {
    //std::cout << "W: " << query << std::endl;

    this->output(query);
    this->output("\n");

    std::vector<std::string> lines;
    while (true) {
        std::string line = this->input();
        //std::cout << "R: " << line << std::endl;
        if (line.empty()) break;
        lines.push_back(line);
    }

    return lines;
}
