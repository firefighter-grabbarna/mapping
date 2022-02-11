#pragma once

#include "window.hpp"
#include "../math.hpp"

class Canvas {
public:
    // Creates a new canvas for the specified window.
    //
    // The center will be at the center of the window, and scale will be the size of
    // the shorter of the vertical and horizontal height.
    Canvas(Window *window, Point center, float scale);

    // Draws a single pixel at the specified coordinate.
    void point(Point point, Color color);

    // Draws a thin line between the specified coordinates.
    void line(Point p1, Point p2, Color color);

private:
    Window *window;

    // The offset and scale such that `pixel = (coord - origin) * scale - + offset`.
    Vec2 offset;
    float scale;

    // Transformsa point from the coordinate space to the 
    Point coordToPixel(Point coord);
};
