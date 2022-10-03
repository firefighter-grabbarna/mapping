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

int main(int argc, const char **argv) {
    const Map map({
        {{0, 980}, {975, 980}},
        {{975, 980}, {975, 0}},
        {{975, 0}, {250, 0}},
        {{250, 0}, {250, 585}},
        {{250, 585}, {0, 585}},
        {{0, 585}, {0, 980}},
    });

    std::srand(time(0));

    std::optional<Serial> lidarSerial;

    for (int i = 1; i < argc; i++) {
        std::string entryPath = argv[i];

        std::cout << entryPath << std::endl;

        Serial serial(entryPath.c_str());


        auto response = serial.query("SCIP2.0");
        if (response.empty()) continue;

        if (response[0] == "SCIP2.0") {
            lidarSerial.emplace(std::move(serial));
            std::cout << "Lidar connected" << std::endl;
        }
    }

    if (!lidarSerial.has_value()) panic("lidar not connected");
    // if (!cannonSerial.has_value()) panic("cannon not connected");


    Lidar lidar(std::move(lidarSerial.value()));

    while (true) {
        float closestDistance = 9999.0;
        Vec2 closestPoint;

        std::vector<int> distances = lidar.scan();
        for (size_t i = 0; i < distances.size(); i++) {
            if (distances[i] < 0 || distances[i] > 4000) continue;

            float angle = (float) i / distances.size() * 3.141592 * 2;

            if (distances[i] < closestDistance) {
                closestDistance = distances[i];
                closestPoint = Vec2(0, -1).rotate(angle) * distances[i];
            }
        }

        std::cout << closestPoint.x << " " << closestPoint.y << std::endl;
    }



    /*
    
    // The guessed position of the robot.
    Transform guess;

    Window window(width, height, windowName);

    while (!window.shouldClose()) {
        window.fill({ 50, 50, 50 });

        std::vector<Point> scanned;

        std::vector<int> distances = lidar.scan();
        for (size_t i = 0; i < distances.size(); i++) {
            if (distances[i] < 0 || distances[i] > 4000) continue;

            float angle = (float) i / distances.size() * 3.141592 * 2;
            scanned.push_back((Vec2(1, 0).rotate(angle) * distances[i]).point());
        }

        if (scanned.size() > 30) {
            guess = updateTransform(guess, map, scanned);

            float cost = transformCost(guess, map, scanned);
            if (cost > 30.0) {
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
    */

    return 0;
}
