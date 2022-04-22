#include <iostream>
#include <filesystem>
#include <optional>
#include <utility>
#include <cmath>
#include <unistd.h>

#include "../common/lidar.hpp"
#include "../common/motors.hpp"
#include "../common/cannon.hpp"
#include "../common/serial.hpp"
#include "../common/util.hpp"
#include "../common/icp.hpp"
#include "../common/math.hpp"
#include "../common/window.hpp"
#include "../common/canvas.hpp"
#include "../common/pins.hpp"

const int DISTANCE_PIN = 23;
const int ALARM_PIN = 24;

// const Map map({
//     {{0, 980}, {975, 980}},
//     {{975, 980}, {975, 0}},
//     {{975, 0}, {250, 0}},
//     {{250, 0}, {250, 585}},
//     {{250, 585}, {0, 585}},
//     {{0, 585}, {0, 980}},
// });

const Map map({
    {{0, 2400}, {700, 2400}}, 
    {{700, 2400}, {2400, 2400}}, 
    {{2400, 2400}, {2400, 900}}, 
    {{2400, 900}, {2400, 0}}, 
    {{2400, 0}, {1180, 0}}, 
    {{1180, 0}, {0, 0}}, 
    {{0, 0}, {0, 1020}}, 
    {{0, 1020}, {0, 2400}}, 
    {{700, 2400}, {700, 1490}}, 
    {{700, 1490}, {470, 1490}}, 
    {{710, 470}, {710, 1020}}, 
    {{710, 1020}, {0, 1020}}, 
    {{1180, 0}, {1180, 440}}, 
    {{1180, 900}, {2400, 900}}, 
    {{1170, 1380}, {1170, 1930}}, 
    {{1170, 1930}, {1920, 1930}}, 
    {{1920, 1930}, {1920, 1380}}, 
    {{1920, 1380}, {1640, 1380}}, 
});

class Navigator {
public:
    std::optional<Window> window;

    Lidar lidar;
    Motors motors;

    Transform position;

    Navigator(std::optional<Window> &&window, Serial &&lidar, Serial &&motors):
        window(std::move(window)),
        lidar(std::move(lidar)),
        motors(std::move(motors)),
        position() {}

    // Uses the lidar to find the current position. Stops the motors if a full
    // search is needed.
    void updatePosition() {
        std::vector<Point> scanned;
        
        while (true) {
            scanned.clear();

            std::vector<int> distances = this->lidar.scan();
            for (size_t i = 0; i < distances.size(); i++) {
                if (distances[i] < 0 || distances[i] > 4000) continue;

                float angle = (float) i / distances.size() * 3.141592 * 2;
                scanned.push_back((Vec2(0, 1).rotate(angle) * distances[i]).point());
            }

            if (scanned.size() > 30) {
                this->position = updateTransform(this->position, map, scanned);

                float cost = transformCost(this->position, map, scanned);
                std::cout << "Cost: " << cost << std::endl;
                if (cost > 50.0) {
                    this->motors.setSpeed({0, 0}, 0);
                    std::cout << "Desync detected" << std::endl;
                    this->position = searchTransform(map, scanned);

                    cost = transformCost(this->position, map, scanned);
                    if (cost > 50.0) {
                        std::cout << "Bad data (" << cost << ")" << std::endl;
                        // Rescan, the last scan could not be found
                        continue;
                    }
                }
            }
            break;
        }

        if (window.has_value()) {
            auto &win = window.value();

            if (win.shouldClose()) {
                panic("window closed");
            }

            win.fill({ 50, 50, 50 });

            Canvas canvas(&win, { 1200, 1200 }, 3000);

            for (const Line &line : map.walls) {
                canvas.line(line.p1, line.p2, { 100, 100, 100 });
            }

            {
                Point target = this->position.applyTo(Point(0, 0));
                canvas.line(
                    target - Vec2(30, 30),
                    target + Vec2(30, 30),
                    {255, 127, 127}
                );
                canvas.line(
                    target - Vec2(30, -30),
                    target + Vec2(30, -30),
                    {255, 127, 127}
                );
            }

            for (const Point &point : scanned) {
                // canvas.line(
                //     this->position.applyTo(Point(0, 0)),
                //     this->position.applyTo(point),
                //     {127, 255, 127}
                // );

                Point target = this->position.applyTo(point);
                canvas.line(
                    target - Vec2(10, 10),
                    target + Vec2(10, 10),
                    {127, 255, 127}
                );
                canvas.line(
                    target - Vec2(10, -10),
                    target + Vec2(10, -10),
                    {127, 255, 127}
                );
            }

            win.redraw();
        }
    }

    void runMotors(Vec2 dirSpeed, float rotSpeed) {
        this->motors.setSpeed(dirSpeed.rotate(-this->position.rotation), rotSpeed);
    }
};

// Drives to the specified point. Returns true if the point was reached,
// false if the fire alarm was triggered.
bool runToPoint(Point target, Navigator &navigator) {
    while (true) {
        navigator.updatePosition();

        if (pinRead(ALARM_PIN) == 1) {
            return false;
        }

        Point robotPos = navigator.position.offset.point();
            //+ Vec2(0, 100).rotate(navigator.position.rotation);

        Vec2 direction = target - robotPos;
        
        std::cout << direction.x << " " << direction.y << std::endl;

        Vec2 dirSpeed = direction / 70;
        if (dirSpeed.mag() > 1) {
            dirSpeed = dirSpeed.normalize();
            navigator.runMotors(dirSpeed, 0);
        } else {
            return true;
        }
    }
}

int main(int argc, const char** argv) {

    pinMode(DISTANCE_PIN, "in"); // avstånd
    pinMode(ALARM_PIN, "in"); // brandvarnare

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
    if (!cannonSerial.has_value()) panic("cannon not connected");

    // Serial cannon(std::move(cannonSerial.value()));

    std::optional<Window> window;

    if (false) {
        Window win(900, 600, "robot");
        window.emplace(std::move(win));
    }

    Navigator navigator(
        std::move(window),
        std::move(lidarSerial.value()),
        std::move(motorsSerial.value())
    );


    Cannon cannon(std::move(cannonSerial.value()));

    std::cout << "Locating" << std::endl;
    navigator.updatePosition();
    std::cout << "Position found" << std::endl;

    while (true) {
        cannon.waitForButton();
        std::cout << "Button pressed" << std::endl;
        usleep(1'000'000);
        std::cout << "Moving to the point" << std::endl;
        Point target(800, 1200);
        bool reached = runToPoint(target, navigator);
        navigator.runMotors({ 0, 0 }, 0);

        if (!reached) {
            std::cout << "Fire found" << std::endl;
            cannon.aimAndShoot();
        }
    }

    // while (!navigator.window.has_value() || !navigator.window.value().shouldClose()) {

    //     navigator.updatePosition();

    //     Point robotPos = navigator.position.offset.point();
    //         //+ Vec2(0, 100).rotate(navigator.position.rotation);

        
    //     // Vec2 direction = target - robotPos;

    //     // Vec2 dirSpeed = direction / 100;
    //     // if (dirSpeed.mag() > 1) {
    //     //     dirSpeed = dirSpeed / dirSpeed.mag();
    //     // }

        
    //     // // //float pi = 3.141592654;
    //     // // float angleDiff = navigator.position.rotation.radians;

    //     // // float rotSpeed = angleDiff * -1;
    //     // // if (rotSpeed > 1) rotSpeed = 1;
    //     // // if (rotSpeed < -1) rotSpeed = -1;
        
    //     // float rotSpeed = 0;

    //     Vec2 dirSpeed(0, 0);
    //     float rotSpeed = 0;
    //     navigator.runMotors(dirSpeed, rotSpeed);
    // }    
}
