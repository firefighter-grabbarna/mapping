#include "../common/icp.hpp"
#include "../common/lidar.hpp"
#include "../common/math.hpp"
#include "../common/util.hpp"
#include "../common/window.hpp"
#include "../common/canvas.hpp"

#include <array>
#include <cmath>
#include <iostream>
#include <map>
#include <vector>

const int width = 900;
const int height = 600;
const char* windowName = "Lidar";

int main() {
    const Map map({
        {{1270, 0}, {1270, 500}},
        {{0, 0}, {1270, 0}},
        {{0, 0}, {0, 620}},
        {{0, 620}, {620, 620}},
        {{620, 620}, {620, 200}},
        {{620, 200}, {730, 200}},
    });

    std::srand(time(0));

    // Connect to the lidar.
    Lidar lidar("/dev/ttyACM0");
    
    // The guessed position of the robot.
    Transform guess;

    Window window(width, height, windowName);

    while (!window.shouldClose()) {
        window.fill({ 50, 50, 50 });

        std::vector<Point> scanned;

        std::vector<int> distances = lidar.scan();
        for (size_t i = 0; i < distances.size(); i++) {
            if (distances[i] < 0 || distances[i] > 4000) continue;

            float angle = -(float) i / distances.size() * 3.141592 * 2;
            scanned.push_back((Vec2(1, 0).rotate(angle) * distances[i]).point());
        }

        if (scanned.size() > 30) {
            guess = updateTransform(guess, map, scanned);

            float cost = transformCost(guess, map, scanned);
            if (cost > 40.0) {
                std::cout << "Desync detected (" << cost << ")" << std::endl << std::endl;
                guess = searchTransform(map, scanned);
            }
            std::cout << "\x1b[A\x1b[K" "Cost: " << cost << std::endl;
        }

        Canvas canvas(&window, { 635, 310 }, 1500);

        for (const Line &line : map.walls) {
            canvas.line(line.p1, line.p2, { 100, 100, 100 });
        }

        for (const Point &point : scanned) {
            canvas.line(
                guess.applyTo(Point(0, 0)),
                guess.applyTo(point),
                {127, 255, 127}
            );
        }

        window.redraw();
    }

    return 0;
}
