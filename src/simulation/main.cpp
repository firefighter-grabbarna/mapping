#include "../common/math.hpp"
#include "../common/util.hpp"
#include "../common/window/window.hpp"
#include "../common/window/canvas.hpp"

#include <array>
#include <cmath>
#include <iostream>
#include <map>
#include <vector>

const int width = 600;
const int height = 600;
const char* windowName = "Simulering";

const std::vector<Line> lines = {
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

float distanceToLines(const std::vector<Line> &lines, Point point) {
    float closest = INFINITY;

    for (const auto &line : lines) {
        Point closestPoint = line.pointClosestTo(point);
        float dist = (closestPoint - point).mag();
        if (dist < closest) closest = dist;
    }

    return closest;
}

void drawDistances(Window &window, const std::vector<Line>& lines) {
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            Vec2 pixel(x - 60, y - 60);
            Point point = Point(pixel / 2 * 10);

            float dist = distanceToLines(lines, point);

            unsigned char color = 50;

            if (dist < 10) {
                color = 0;
            } else if (dist < 200) {
                color = 250 - dist;
            }

            window.put(x, y, { color, color, color });
        }
    }
}

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

void addRandomOffsets(std::vector<Line> &lines, float intensity) {
    std::map<std::pair<int, int>, Point> cache;
    for (Line &line : lines) {
        addRandomOffset(line.p1, intensity, cache);
        addRandomOffset(line.p2, intensity, cache);
    }
}

int main() {
    Window window(width, height, windowName);

    std::vector<Line> distorted = lines;
    addRandomOffsets(distorted, 50);

    while (!window.shouldClose()) {
        window.fill({ 50, 50, 50 });

        //drawDistances(window, distorted);

        Canvas canvas(&window, { 1200, 1200 }, 3000);

        for (const Line &line : lines) {
            canvas.line(line.p1, line.p2, { 150, 150, 150 });
        }

        for (const Line &line : distorted) {
            canvas.line(line.p1, line.p2, { 255, 255, 255 });
        }

        for (float angle = 0; angle <= 6.28; angle += 0.05) {
            Point origin(800, 1200);
            Vec2 direction(sinf(angle), cosf(angle));
            Ray ray(origin, direction);

            Point closest;
            float closestDist = 1e100;

            for (const Line &line : distorted) {
                auto result = ray.castOnto(line);
                if (!result.has_value()) continue;
                Point hit = result.value();

                float dist = (hit - origin).mag();
                if (dist > closestDist) continue;
                
                closest = hit;
                closestDist = dist;
            }

            canvas.line(origin, closest, {255, 127, 127});
        }

        window.redraw();
    }

    return 0;
}
