#include "../common/math.hpp"
#include "../common/util.hpp"
#include "../common/window/window.hpp"

#include <iostream>
#include <cmath>

const int width = 600;
const int height = 600;
const char* windowName = "Simulering";

Vec2 lines[][2] = {
    {{0, 0}, {2400, 0}},
    {{0, 0}, {0, 2400}},
    {{2400, 0}, {2400, 2400}},
    {{2400, 2400}, {0, 2400}},
    {{700, 0}, {700, 910}},
    {{700, 910}, {470, 910}},
    {{710, 1930}, {710, 1390}},
    {{710, 1390}, {0, 1390}},
    {{1180, 2400}, {1180, 1960}},
    {{1180, 1500}, {2400, 1500}},
    {{1170, 1020}, {1170, 470}},
    {{1170, 470}, {1920, 470}},
    {{1920, 470}, {1920, 1020}},
    {{1920, 1020}, {1640, 1020}},
};

float distance_to_lines(Vec2 point) {
    float closest = INFINITY;

    for (unsigned long i = 0; i < sizeof(lines) / sizeof(lines[0]); i++) {
        Vec2 p1 = lines[i][0];
        Vec2 p2 = lines[i][1];
    
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

void redraw(Window &window) {
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            Vec2 pixel(x - 60, height - y - 60);
            Vec2 point = pixel / 2 * 10;

            float dist = distance_to_lines(point);

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

int main() {
    Window window(width, height, windowName);

    while (!window.shouldClose()) {
        redraw(window);
        window.redraw();
    }

    return 0;
}
