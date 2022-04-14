#include <iostream>
#include <string>

#include "../common/serial.hpp"
#include "../common/util.hpp"

int main(int argc, const char** argv) {
    for (int i = 1; i < argc; i++) {
        std::string entryPath = argv[i];

        std::cout << entryPath << std::endl;

        Serial serial(entryPath.c_str());

        auto response = serial.query("SCIP2.0");
        if (response.empty()) continue;

        if (response[0] == "MOTOR") {
            serial.output("0 0 0\n");
            return 0;
        }
    }
    panic("motors not connected");
}
