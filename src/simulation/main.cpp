#include "../common/icp.hpp"
#include "../common/math.hpp"
#include "../common/util.hpp"
#include "../common/window/window.hpp"
#include "../common/window/canvas.hpp"

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

const int scanResolution = 512;

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

        Ray ray({0, 0}, { angle });
        ray = transform.applyTo(ray);

        Point closest;
        float closestDist = 1e100;

        for (const Line &wall : map.walls) {
            auto result = ray.castOnto(wall);
            if (!result.has_value()) continue;

            float dist = (result.value() - ray.origin).mag();
            if (dist > closestDist) continue;
            
            closest = result.value();
            closestDist = dist;
        }

        if (closestDist < 4096 && closestDist > 250) {
            points.push_back(transform.inverse().applyTo(closest));
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

    //std::srand(1);
    std::srand(time(0));

    // Create the distorted map
    Map distorted = map;
    addRandomOffsets(distorted, 50);

    float time = 0;

    // The actual poisiton and rotation of the robot.
    Transform realTransform(time * 6.28 / 3, Vec2(1200, 1200) + Vec2(800, 0).rotate(time / 4));
    Transform guess = realTransform;

    Window window(width, height, windowName);


    while (!window.shouldClose()) {
        window.fill({ 50, 50, 50 });

        realTransform = Transform(time * 6.28 / 3, Vec2(1200, 1200) + Vec2(800, 0).rotate(time / 4));

        // Simulate a scan in the distorted map, and use it to update the position
        std::vector<Point> scanned = simulateScan(distorted, realTransform);
        guess = updateTransform(guess, map, scanned);

        Canvas leftCanvas(&window, { 2600, 1200 }, 4000);
        Canvas rightCanvas(&window, { -200, 1200 }, 4000);

        for (const Line &line : distorted.walls) {
            leftCanvas.line(line.p1, line.p2, { 255, 255, 255 });
        }

        for (const Line &line : map.walls) {
            rightCanvas.line(line.p1, line.p2, { 100, 100, 100 });
        }

        for (const Point &point : scanned) {
            leftCanvas.line(
                realTransform.applyTo(Point(0, 0)),
                realTransform.applyTo(point),
                {255, 127, 127}
            );
            
            rightCanvas.line(
                guess.applyTo(Point(0, 0)),
                guess.applyTo(point),
                {127, 255, 127}
            );
        }

        time += 1.0 / 60;

        window.redraw();
    }

    return 0;
}
