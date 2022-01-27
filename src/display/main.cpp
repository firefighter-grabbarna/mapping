#include "../common/lidar.hpp"
#include "../common/util.hpp"
#include "window.hpp"

#include <cmath>
#include <iostream>
#include <vector>

const int width = 1024;
const int height = 640;
const char* windowName = "Firefighter";

void redraw(Window &window, const std::vector<int> &values) {
    int diagonal = sqrt(window.width * window.width + window.height * window.height);

    float max_dist = 5000.0;
    float scale = max_dist / (diagonal / 2);

    for (int y = 0; y < window.height; y++) {
        for (int x = 0; x < window.width; x++) {
            int dx = x - window.width / 2;
            int dy = y - window.height / 2;
            int dist = sqrt(dx * dx + dy * dy);

            float radians = atan2(-dx, dy) + 3.141592654;
            size_t index = (size_t) (radians / (2 * 3.141592654) * values.size());
            if (index >= values.size()) index = values.size() - 1;

            int value = values[index];
            //if (value < 200) value = 0;

            if (value == -1) {
                window.put(x, y, 255, 0, 0);
            } else if (value < 200) {
                window.put(x, y, 127, 127, 127);
            } else if (dist > value / scale + 5) {
                window.put(x, y, 127, 127, 127);
            } else if (dist > value / scale) {
                window.put(x, y, 0, 0, 0);
            } else {
                window.put(x, y, 255, 255, 255);
            }
        }
    }
}

int main() {
    // Connect to the lidar.
    Lidar lidar("/dev/ttyACM0");
    
    Window window(width, height, windowName);

    while (!window.shouldClose()) {
        auto values = lidar.scan();
        redraw(window, values);
        window.redraw();
    }

    return 0;
}
