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
#include "../common/nodemap.hpp"

const int DISTANCE_PIN = 23;
//const int ALARM_PIN = 24;

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
    NodeMap nodeMap;

    time_t lastFailure;

    Navigator(std::optional<Window> &&window, Serial &&lidar, Serial &&motors):
        window(std::move(window)),
        lidar(std::move(lidar)),
        motors(std::move(motors)),
        position(),
        nodeMap(),
        lastFailure(std::time(nullptr)) {}

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
                bool shouldFail = std::time(nullptr) > lastFailure + 30;

                this->position = updateTransform(this->position, map, scanned);

                float cost = transformCost(this->position, map, scanned);
                std::cout << "Cost: " << cost << std::endl;
                if (cost > 50.0 || shouldFail) {
                    lastFailure = std::time(nullptr);
                    this->motors.setSpeed({0, 0}, 0);
                    std::cout << "Desync detected" << std::endl;
                    this->position = searchTransform(map, scanned);

                    cost = transformCost(this->position, map, scanned);
                    if (cost > 50.0 || shouldFail) {
                        std::cout << "Bad data (" << cost << ")" << std::endl;

                        this->motors.setSpeed({0, 0}, 1);
                        int delay = (int) ((long long) rand() * 500000 / RAND_MAX + 500000);
                        usleep(delay);
                        this->motors.setSpeed({0, 0}, 0);
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
void runToPoint(Point target, Navigator &navigator) {
    while (true) {
        navigator.updatePosition();

        Point robotPos = navigator.position.offset.point()
            + Vec2(0, 45).rotate(navigator.position.rotation);

        Vec2 direction = target - robotPos;
        
        std::cout << direction.x << " " << direction.y << std::endl;

        Vec2 dirSpeed = direction / 70;
        if (dirSpeed.mag() > 1) {
            dirSpeed = dirSpeed.normalize();
            navigator.runMotors(dirSpeed, 0);
        } else {
            return;
        }
    }
}

void rotateToAngle(Rotation angle, Navigator &navigator) {
    bool didChangeMotors = false;
    while (true) {
        navigator.updatePosition();

        float angleDiff = angle.radians - navigator.position.rotation.radians;

        if (angleDiff > 3.141592) angleDiff -= 3.141592 * 2;
        if (angleDiff < -3.141592) angleDiff += 3.141592 * 2;

        if (angleDiff > 0.5) {
            navigator.runMotors({0, 0}, angleDiff);
        } else if (angleDiff < -0.5) {
            navigator.runMotors({0, 0}, angleDiff);
        } else {
            if (didChangeMotors)
                navigator.runMotors({ 0, 0 }, 0);
            break;
        }
        didChangeMotors = true;
    }
}

void followLineStep(Line line, Navigator &navigator) {
    navigator.updatePosition();

    Point robotPos = navigator.position.offset.point()
        + Vec2(0, 45).rotate(navigator.position.rotation);

    Point closest = line.pointClosestTo(robotPos);

    Vec2 towardsEnd = (line.p2 - robotPos).normalize();
    Vec2 towardsLine = (closest - robotPos).normalize();

    float distance = (closest - robotPos).mag();
    float factor = std::min(distance / 200.0, 1.0);

    Vec2 direction = towardsEnd + (towardsLine - towardsEnd) * factor;

    std::cout << direction.x << " " << direction.y << std::endl;

    navigator.runMotors(direction.normalize(), 0);
}

void moveToRoom(int room, Navigator &navigator) {
    navigator.updatePosition();
    while (true) {
        Point robotPos = navigator.position.offset.point()
            + Vec2(0, 45).rotate(navigator.position.rotation);
        Node *current = navigator.nodeMap.getCurrentNode(robotPos.x, robotPos.y);
        Node *target = navigator.nodeMap.getRoomNode(room);

        if (current == target) break;

        std::vector<Point> path = navigator.nodeMap.getPath(current, target);
        followLineStep({ path[0], path[1] }, navigator);
    }
    navigator.runMotors({ 0, 0 }, 0);
}

bool moveToCandle(Cannon &cannon, Navigator &navigator) {
    navigator.updatePosition();

    int angle = cannon.getFireAngle();
    std::cout << angle << std::endl;
    if (angle == -1) return false;

    time_t startTime = std::time(nullptr);

    Point robotPos = navigator.position.offset.point()
        + Vec2(0, 45).rotate(navigator.position.rotation);
    float robotAngle = navigator.position.rotation.radians;
    float totalAngle = robotAngle + (angle - 90) * (3.141592 / 180.0);

    std::cout << "A: " << robotAngle << " " << totalAngle << std::endl;

    Line line(robotPos, robotPos + Vec2(0, 10000).rotate(totalAngle));

    bool didSeeFire = false;

    Point lastCheck = robotPos;

    while (true) {
        if (std::time(nullptr) > startTime + 90) return false;
        navigator.lastFailure = std::time(nullptr);

        rotateToAngle(totalAngle, navigator);

        Point robotPos = navigator.position.offset.point()
            + Vec2(0, 45).rotate(navigator.position.rotation);
            
        if ((robotPos - lastCheck).mag() > 50) {
            lastCheck = robotPos;
            navigator.runMotors({0, 0}, 0);
            usleep(1'000'000);
            navigator.updatePosition();
        }

        bool seesFireNow = pinRead(DISTANCE_PIN);
        std::cout << (seesFireNow ? "fire" : "no fire") << std::endl;
        if (didSeeFire && !seesFireNow) break;
        didSeeFire = seesFireNow;

        robotPos = navigator.position.offset.point()
            + Vec2(0, 45).rotate(navigator.position.rotation);

        Vec2 awayFromWall = map.closestPointTo(robotPos) - robotPos;

        Vec2 towardsRay = line.pointClosestTo(robotPos) - robotPos;
        Vec2 alongRay = line.p2 - line.p1;

        float rayDist = towardsRay.mag();
        float rayFactor = std::min(rayDist / 200.0, 1.0);

        float wallDist = awayFromWall.mag();
        float wallFactor = std::max(std::min((200.0 - wallDist) / 50.0, 1.0), 0.0);

        Vec2 direction = alongRay.normalize();
        direction = direction + (towardsRay.normalize() - direction) * rayFactor;
        direction = direction + (awayFromWall.normalize() - direction) * wallFactor;

        float angleDiff = totalAngle - navigator.position.rotation.radians;

        if (angleDiff > 3.141592) angleDiff -= 3.141592 * 2;
        if (angleDiff < -3.141592) angleDiff += 3.141592 * 2;

        direction = direction.normalize() * 0.7;
        navigator.runMotors(direction, angleDiff);
    }
    navigator.runMotors({0, 0}, 0);
    return true;
}

/*void followPath(std::vector<Point> path, Navigator &navigator) {
    for (size_t i = 0; i < path.size() - 1; i++) {
        Line line(path[i], path[i + 1]);
        followLine(line, navigator);
    }
    navigator.runMotors({0, 0}, 0);
}*/

int main(int argc, const char** argv) {
    std::srand(time(0));

    pinMode(DISTANCE_PIN, "in"); // avstånd
    //pinMode(ALARM_PIN, "in"); // brandvarnare

    // std::cout << "distance: " << pinRead(DISTANCE_PIN) << std::endl;
    // std::cout << "alarm: " << pinRead(ALARM_PIN) << std::endl;

    // if (true) return 0;

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

    /*std::cout << "Locating" << std::endl;
    navigator.updatePosition();
    std::cout << "Position found" << std::endl;*/

    while (true) {
        cannon.waitForButton();
        std::cout << "Button pressed" << std::endl;
        usleep(1'000'000);
        std::cout << "Following path" << std::endl;

        // if (moveToCandle(cannon, navigator)) {
        //     std::cout << "Fire reached" << std::endl;

        //     cannon.aimAndShoot();
        // }
        // continue;

        while (true) {
            moveToRoom(2, navigator);
            rotateToAngle(-0.5, navigator);

            if (moveToCandle(cannon, navigator)) {
                cannon.aimAndShoot();
                continue;
            }

            usleep(1'000'000);

            moveToRoom(1, navigator);
            rotateToAngle(-0.75, navigator);

            if (moveToCandle(cannon, navigator)) {
                cannon.aimAndShoot();
                continue;
            }

            usleep(1'000'000);

            moveToRoom(4, navigator);
            rotateToAngle(-2, navigator);

            if (moveToCandle(cannon, navigator)) {
                cannon.aimAndShoot();
                continue;
            }

            usleep(1'000'000);

            moveToRoom(3, navigator);
            rotateToAngle(0.75, navigator);

            if (moveToCandle(cannon, navigator)) {
                cannon.aimAndShoot();
                continue;
            }

            usleep(1'000'000);
        }

        // std::vector<Point> path = navigator.nodeMap.getPath(current, target);
        // path.insert(path.begin(), robotPos);

        // for (Point p : path) {
        //     std::cout << "Point " << p.x << " " << p.y << std::endl;
        // }
        
        // followPath(path, navigator);



        /*Point target(800, 1200);
        bool reached = runToPoint(target, navigator);
        navigator.runMotors({ 0, 0 }, 0);

        if (!reached) {
            std::cout << "Fire found" << std::endl;
            cannon.aimAndShoot();
        }*/
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
