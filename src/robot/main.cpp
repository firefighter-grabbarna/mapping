#include <iostream>

#include "../common/lidar.hpp"

using namespace std;

int main() {
    Lidar lidar("/dev/ttyACM0");

    while (true) {
        auto values = lidar.scan();
        for (int v : values) {
            std::cout << v << " ";
        }
        std::cout << std::endl << std::endl;
    }
}
