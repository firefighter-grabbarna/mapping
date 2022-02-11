#include "canvas.hpp"
#include "window.hpp"
#include "../math.hpp"

#include <algorithm>

Canvas::Canvas(Window *window, Vec2 center, float scale) {
    this->window = window;

    Vec2 windowSize(window->width, window->height);
    this->delta = std::min(windowSize.x, windowSize.y) / scale;
    this->origin = windowSize / 2 - center * this->delta;
}

void Canvas::point(Vec2 pos, Color color) {
    Vec2 pixel = this->origin + pos * this->delta;

    this->window->put(pixel.x, pixel.y, color);
}

void Canvas::line(Vec2 p1, Vec2 p2, Color color) {
    p1 = this->origin + p1 * this->delta;
    p2 = this->origin + p2 * this->delta;

    this->window->line(p1.x, p1.y, p2.x, p2.y, color);
}
