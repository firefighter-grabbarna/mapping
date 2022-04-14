#include "../common/icp.hpp"
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
const char* windowName = "Simulering";

const float minScanAngle = -120 * (3.141592 / 180);
const float maxScanAngle = 120 * (3.141592 / 180);

const int scanResolution = 700;

// Generates a random float in the specified range.
static float randomFloat(float min, float max) {
    return min + (max - min) / RAND_MAX * rand();
}

// Adds a random offset with the specified intensity to the point. The cache is
// used to ensure that the same point is always move to the same position.
static void addRandomOffset(Point &point, float intensity, std::map<std::pair<int, int>, Point> &cache) {
    std::pair<int, int> key(point.x, point.y);
    if (cache.find(key) == cache.end()) {
        Vec2 offset(randomFloat(-1, 1), randomFloat(-1, 1));
        cache[key] = point + offset * intensity;
    }
    point = cache[key];
}

// Distorts the map by adding random offsets to the endpoints of all lines.
static void addRandomOffsets(Map &map, float intensity) {
    std::map<std::pair<int, int>, Point> cache;
    for (Line &wall : map.walls) {
        addRandomOffset(wall.p1, intensity, cache);
        addRandomOffset(wall.p2, intensity, cache);
    }
}

// Simulates a scan and returns the point cloud relative to the robot.
static std::vector<Point> simulateScan(const Map &map, const Transform &transform) {
    std::vector<Point> points;

    for (int i = 0; i < scanResolution; i++) {
        float angle = minScanAngle + i * (maxScanAngle - minScanAngle) / scanResolution;

        Ray ray({ 0, 0 }, { angle });
        ray = transform.applyTo(ray);

        Point hit = map.raycast(ray);
        float dist = (hit - ray.origin).mag();

        if (dist < 4096 && dist > 250) {
            points.push_back(transform.inverse().applyTo(hit));
        }
    }

    return points;
}

int main() {
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

    const std::vector<Point> path = {
        { 800, 1200 },
        { 2200, 1200 },
        { 2200, 2200 },
        { 1000, 2200, },
        { 1000, 200, },
        { 200, 200, },
    };

    size_t pathIndex = 0;

    //std::srand(1);
    std::srand(time(0));

    // Create the distorted map
    Map distorted = map;
    addRandomOffsets(distorted, 50);

    float time = 0;

    // The actual poisiton and rotation of the robot.
    Transform realTransform(0, { 1000, 1000 });

    // The guessed position of the robot.
    Transform guess;

    Window window(width, height, windowName);


    while (!window.shouldClose()) {
        window.fill({ 50, 50, 50 });


        // Simulate a scan in the distorted map, and use it to update the position
        std::vector<Point> scanned = simulateScan(distorted, realTransform);
        if (scanned.size() > 30) {
            guess = updateTransform(guess, map, scanned);

            float cost = transformCost(guess, map, scanned);
            if (cost > 40.0) {
                std::cout << "Desync detected (" << cost << ")" << std::endl << std::endl;
                guess = searchTransform(map, scanned);
            }
            std::cout << "\x1b[A\x1b[K" "Cost: " << cost << std::endl;


            Line currentLine(path[pathIndex], path[pathIndex + 1]);

            Point position = guess.offset.point();
            Point closestPoint = currentLine.pointClosestTo(position);
            Vec2 offsetTowardsLine = (closestPoint - position).normalize();
            Vec2 offsetOnLine = (currentLine.p2 - position).normalize();

            if ((position - currentLine.p2).mag() < 100.0 && pathIndex < path.size() - 2) {
                pathIndex += 1;
            }

            float distance = (closestPoint - position).mag();
            float factor = std::min(distance / 200.0, 1.0);

            Vec2 offset = offsetOnLine + (offsetTowardsLine - offsetOnLine) * factor;
            offset = offset.normalize();

            realTransform.offset = realTransform.offset + offset * (250.0 / 60.0);
        }


        Canvas leftCanvas(&window, { 2600, 1200 }, 4000);
        Canvas rightCanvas(&window, { -200, 1200 }, 4000);

        for (const Line &line : distorted.walls) {
            leftCanvas.line(line.p1, line.p2, { 255, 255, 255 });
        }

        for (const Line &line : map.walls) {
            rightCanvas.line(line.p1, line.p2, { 100, 100, 100 });
        }

        for (const Point &point : scanned) {
            Point target1 = realTransform.applyTo(point);
            leftCanvas.line(
                target1 - Vec2(10, 10),
                target1 + Vec2(10, 10),
                {255, 127, 127}
            );
            leftCanvas.line(
                target1 - Vec2(10, -10),
                target1 + Vec2(10, -10),
                {255, 127, 127}
            );
            
            Point target2 = guess.applyTo(point);
            rightCanvas.line(
                target2 - Vec2(10, 10),
                target2 + Vec2(10, 10),
                {127, 255, 127}
            );
            rightCanvas.line(
                target2 - Vec2(10, -10),
                target2 + Vec2(10, -10),
                {127, 255, 127}
            );
        }

        leftCanvas.line(
            realTransform.offset.point() - Vec2(30, 30),
            realTransform.offset.point() + Vec2(30, 30),
            {255, 127, 127}
        );
        leftCanvas.line(
            realTransform.offset.point() - Vec2(30, -30),
            realTransform.offset.point() + Vec2(30, -30),
            {255, 127, 127}
        );

        rightCanvas.line(
            guess.offset.point() - Vec2(30, 30),
            guess.offset.point() + Vec2(30, 30),
            {127, 255, 127}
        );
        rightCanvas.line(
            guess.offset.point() - Vec2(30, -30),
            guess.offset.point() + Vec2(30, -30),
            {127, 255, 127}
        );

        for (size_t i = 0; i < path.size() - 1; i++) {
            rightCanvas.line(path[i], path[i + 1], { 128, 128, 255 });
        }

        time += 1.0 / 60;

        window.redraw();
    }

    return 0;
}
