#include <iostream>

#include "../common/lidar.hpp"
#include "../common/serial.hpp"

int main() {
    Serial serial("/dev/ttyAMA0");

    std::string line;

    bool on = false;

    while (getline(std::cin, line)) {
        // do something with the line

        on = !on;
        serial.output(on ? "1\n" : "0\n");
        std::cout << "W: " << (int) on << std::endl;
    }

    //serial.output("1\n");

    // Lidar lidar("/dev/ttyACM0");

    // while (true) {
    //     auto values = lidar.scan();
    //     for (int v : values) {
    //         std::cout << v << " ";
    //     }
    //     std::cout << std::endl << std::endl;
    // }
}
