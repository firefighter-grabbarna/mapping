#include <iostream>
#include <filesystem>
#include <optional>
#include <utility>
#include <cmath>

#include "../common/lidar.hpp"
#include "../common/motors.hpp"
#include "../common/serial.hpp"
#include "../common/util.hpp"

#include "../common/icp.hpp"
#include "../common/math.hpp"
#include "../common/window.hpp"
#include "../common/canvas.hpp"

int main(int argc, const char** argv) {
    std::optional<Serial> lidarSerial;
    std::optional<Serial> motorsSerial;
    std::optional<Serial> cannonSerial;

    for (int i = 1; i < argc; i++) {
        std::string entryPath = argv[i];

        std::cout << entryPath << std::endl;

        Serial serial(entryPath.c_str());


        auto response = serial.query("SCIP2.0");
        if (response.empty()) continue;

        if (response[0] == "SCIP2.0") {
            lidarSerial.emplace(std::move(serial));
            std::cout << "Lidar connected" << std::endl;
        } else if (response[0] == "MOTOR") {
            motorsSerial.emplace(std::move(serial));
            std::cout << "Motors connected" << std::endl;
        } else if (response[0] == "CANNON") {
            cannonSerial.emplace(std::move(serial));
            std::cout << "Cannon connected" << std::endl;
        }
    }

    if (!lidarSerial.has_value()) panic("lidar not connected");
    if (!motorsSerial.has_value()) panic("motors not connected");
    // if (!cannonSerial.has_value()) panic("cannon not connected");

    Lidar lidar(std::move(lidarSerial.value()));
    // Serial motors(std::move(motorsSerial.value()));
    Motors motors(std::move(motorsSerial.value()));
    // Serial cannon(std::move(cannonSerial.value()));

    /*
    while (true) {
        std::vector<int> distances = lidar.scan();

        float bestAngle = 0;
        int bestDist = 4000;

        for (size_t i = 0; i < distances.size(); i++) {
            if (distances[i] < 0 || distances[i] > 4000) continue;

            float angle = (float) i / distances.size() * 3.141592 * 2;

            if (distances[i] < bestDist) {
                bestDist = distances[i];
                bestAngle = angle;
            }
        }

        Vec2 direction = Vec2(0.0, -1.0).rotate(bestAngle);
        float rotVel = (bestAngle - 3.141592) / 2;

        motors.setSpeed(-direction, 0);

        std::cout << bestAngle << std::endl;

    }*/

    const Map map({
        {{0, 980}, {975, 980}},
        {{975, 980}, {975, 0}},
        {{975, 0}, {250, 0}},
        {{250, 0}, {250, 585}},
        {{250, 585}, {0, 585}},
        {{0, 585}, {0, 980}},
    });

    Transform guess;

    Point target(600, 600);

    //Window window(900, 600, "robot");

    //while (!window.shouldClose()) {
    while (true) {
        //window.fill({ 50, 50, 50 });

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
                motors.setSpeed({0, 0}, 0);
                std::cout << "Desync detected (" << cost << ")" << std::endl << std::endl;
                guess = searchTransform(map, scanned);
            }
            std::cout << "\x1b[A\x1b[K" "Cost: " << cost << std::endl;
        }

        Vec2 direction = guess.offset.point() - target;

        Vec2 dirSpeed = direction / 100;
        if (dirSpeed.mag() > 1)
            dirSpeed = dirSpeed / dirSpeed.mag();

        float pi = 3.141592654;
        float angleDiff = guess.rotation.radians;

        float rotSpeed = angleDiff * -1;
        if (rotSpeed > 1) rotSpeed = 1;
        if (rotSpeed < -1) rotSpeed = -1;

        motors.setSpeed(dirSpeed.rotate(-guess.rotation + 3.1415/2), rotSpeed);
        /*
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
        */
    }
    
}
