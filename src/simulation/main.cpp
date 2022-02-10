#include "../common/math.hpp"
#include "../common/util.hpp"
#include "../common/window/window.hpp"

#include <array>
#include <cmath>
#include <iostream>
#include <map>
#include <vector>

const int width = 600;
const int height = 600;
const char* windowName = "Simulering";

struct Line {
    Vec2 p1, p2;
};

const std::vector<Line> lines = {
    {{0,0},{700,0}},
    {{700,0},{2400,0}},
    {{2400,0},{2400,1500}},
    {{2400,1500},{2400,2400}},
    {{2400,2400},{1180,2400}},
    {{1180,2400},{0,2400}},
    {{0,2400},{0,1380}},
    {{0,1380},{0,0}},
    {{700,0},{700,910}},
    {{700,910},{470,910}},
    {{710,1930},{710,1380}},
    {{710,1380},{0,1380}},
    {{1180,2400},{1180,1960}},
    {{1180,1500},{2400,1500}},
    {{1170,1020},{1170,470}},
    {{1170,470},{1920,470}},
    {{1920,470},{1920,1020}},
    {{1920,1020},{1640,1020}},
};

float distance_to_lines(const std::vector<Line> &lines, Vec2 point) {
    float closest = INFINITY;

    for (const auto &line : lines) {
        Vec2 p1 = line.p1;
        Vec2 p2 = line.p2;
    
        Vec2 lv = p2 - p1;
        Vec2 pv = point - p1;

        float fraction = lv.dot(pv) / lv.dot(lv);

        if (fraction > 1) fraction = 1;
        if (fraction < 0) fraction = 0;

        Vec2 p3 = p1 + lv * fraction;

        float dist = (p3 - point).mag();
        if (dist < closest) closest = dist;
    }

    return closest;
}

void redraw(Window &window, const std::vector<Line>& lines) {
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            Vec2 pixel(x - 60, height - y - 60);
            Vec2 point = pixel / 2 * 10;

            float dist = distance_to_lines(lines, point);

            int color = 50;

            if (dist < 10) {
                color = 0;
            } else if (dist < 200) {
                color = 250 - dist;
            }

            window.put(x, y, color, color, color);
        }
    }
}

void addRandomOffset(Vec2 &point, float intensity, std::map<std::pair<int, int>, Vec2> &cache) {
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
    std::map<std::pair<int, int>, Vec2> cache;
    for (Line &line : lines) {
        addRandomOffset(line.p1, intensity, cache);
        addRandomOffset(line.p2, intensity, cache);
    }
}

int main() {
    Window window(width, height, windowName);

    std::vector<Line> distorted = lines;
    addRandomOffsets(distorted, 20);

    while (!window.shouldClose()) {
        redraw(window, distorted);
        window.redraw();
    }

    return 0;
}
