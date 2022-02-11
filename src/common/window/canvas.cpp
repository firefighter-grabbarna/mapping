#include "canvas.hpp"
#include "window.hpp"
#include "../math.hpp"

#include <algorithm>

Canvas::Canvas(Window *window, Point center, float scale) {
    this->window = window;

    Vec2 windowSize(window->width, window->height);

    this->scale = std::min(windowSize.x, windowSize.y) / scale;
    this->offset = windowSize / 2 - Vec2(center) * this->scale;
}

void Canvas::point(Point pos, Color color) {
    Point pixel = coordToPixel(pos);

    this->window->put(pixel.x, pixel.y, color);
}

void Canvas::line(Point p1, Point p2, Color color) {
    p1 = this->coordToPixel(p1);
    p2 = this->coordToPixel(p2);

    this->window->line(p1.x, p1.y, p2.x, p2.y, color);
}

Point Canvas::coordToPixel(Point point) {
    Point scaled = Point(Vec2(point) * this->scale);
    return scaled + this->offset;
}