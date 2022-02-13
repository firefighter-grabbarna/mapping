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

const int scanResolution = 256;

// Adds a random offset with the specified intensity to the point. The cache is
// used to ensure that the same point is always move to the same position.
void addRandomOffset(Point &point, float intensity, std::map<std::pair<int, int>, Point> &cache) {
    std::pair<int, int> key(point.x, point.y);
    if (cache.find(key) == cache.end()) {
        Vec2 offset(
            rand() % 2000 / (float) 1000 - 1,
            rand() % 2000 / (float) 1000 - 1
        );
        cache[key] = point + offset * intensity;
    }
    point = cache[key];
}

// Distorts the map by adding random offsets to the endpoints of all lines.
void addRandomOffsets(std::vector<Line> &map, float intensity) {
    std::map<std::pair<int, int>, Point> cache;
    for (Line &line : map) {
        addRandomOffset(line.p1, intensity, cache);
        addRandomOffset(line.p2, intensity, cache);
    }
}

// Simulates a scan and returns the point cloud relative to the robot.
std::vector<Point> simulateScan(const std::vector<Line> &lines, const Transform &transform) {
    std::vector<Point> points;

    for (int i = 0; i < scanResolution; i++) {
        Ray ray({0, 0}, { i * (float) 6.28318530718 / scanResolution });
        ray = transform.applyTo(ray);

        Point closest;
        float closestDist = 1e100;

        for (const Line &line : lines) {
            auto result = ray.castOnto(line);
            if (!result.has_value()) continue;

            float dist = (result.value() - ray.origin).mag();
            if (dist > closestDist) continue;
            
            closest = result.value();
            closestDist = dist;
        }

        if (closestDist < 4096 && closestDist > 200) {
            points.push_back(transform.inverse().applyTo(closest));
        }
    }

    return points;
}

Point closestPointInMap(Point point, const std::vector<Line> &map) {
    Point closest = point;
    float closestDistSq = INFINITY;

    for (const Line& line : map) {
        Point target = line.pointClosestTo(point);
        float distSq = (target - point).magSq();

        if (distSq < closestDistSq) {
            closest = target;
            closestDistSq = distSq;
        }
    }

    return closest;
}

Transform guessTransform(
    const std::vector<Point> &scanned,
    Transform oldGuess,
    const std::vector<Line> &map
) {
    Vec2 translationSum;
    for (const Point &point : scanned) {
        Point target = closestPointInMap(oldGuess.applyTo(point), map);
        translationSum = translationSum + (target - point);
    }
    Vec2 translation = translationSum / scanned.size();

    // To do: Rotation

    return Transform({ 0 }, translation);
}

Transform iterateGuess(
    const std::vector<Point> &scanned,
    Transform guess,
    const std::vector<Line> &map
) {
    float diffThreshold = 1.0;
    float largestDiff;

    do {
        Transform oldGuess = guess;

        guess = guessTransform(scanned, oldGuess, map);

        largestDiff = 0.0;
        for (const Point &point : scanned) {
            float diff = (guess.applyTo(point) - oldGuess.applyTo(point)).mag();
            if (diff > largestDiff)
                largestDiff = diff;
        }
    } while (largestDiff > diffThreshold);

    return guess;
}

float costOfGuess(
    const std::vector<Point> &scanned,
    Transform guess,
    const std::vector<Line> &map
) {
    float sum = 0.0;

    for (const Point &point : scanned) {
        Point transformed = guess.applyTo(point);
        Point closest = closestPointInMap(transformed, map);

        sum += (closest - transformed).magSq();
    }

    return sqrtf(sum) / (scanned.size() - 1);
}

int main() {
    const std::vector<Line> map = {
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
    };

    std::srand(time(0));

    Window window(width, height, windowName);

    // Create the distorted map
    std::vector<Line> distorted = map;
    addRandomOffsets(distorted, 50);

    // The actual poisiton and rotation of the robot.
    Transform realTransform(Rotation(0), Vec2(800, 1300));

    // Simulate a scan in the distorted map
    std::vector<Point> scanned = simulateScan(distorted, realTransform);

    // The guessed transform of the robot
    Transform guess1(Rotation(0), Vec2(1000, 1100));
    Transform guess2 = iterateGuess(scanned, guess1, map);

    std::cout
        << costOfGuess(scanned, guess1, map) << " -> "
        << costOfGuess(scanned, guess2, map) << std::endl;

    while (!window.shouldClose()) {
        window.fill({ 50, 50, 50 });

        Canvas leftCanvas(&window, { 2600, 1200 }, 4000);
        Canvas rightCanvas(&window, { -200, 1200 }, 4000);

        for (const Line &line : distorted) {
            leftCanvas.line(line.p1, line.p2, { 255, 255, 255 });
        }

        for (const Line &line : map) {
            rightCanvas.line(line.p1, line.p2, { 100, 100, 100 });
        }

        for (const Point &point : scanned) {
            leftCanvas.line(
                realTransform.applyTo(Point(0, 0)),
                realTransform.applyTo(point),
                {255, 127, 127}
            );
            
            rightCanvas.line(
                guess1.applyTo(point),
                guess2.applyTo(point),
                {127, 255, 127}
            );
        }

        window.redraw();
    }

    return 0;
}
