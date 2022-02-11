#pragma once

#include "window.hpp"
#include "../math.hpp"

class Canvas {
public:
    // Creates a new canvas for the specified window.
    //
    // The center will be at the center of the window, and scale will be the size of
    // the shorter of the vertical and horizontal height.
    Canvas(Window *window, Vec2 center, float scale);

    // Draws a single pixel at the specified coordinate.
    void point(Vec2 point, Color color);

    // Draws a thin line between the specified coordinates.
    void line(Vec2 p1, Vec2 p2, Color color);

private:
    Window *window;

    // The origin and scale such that `pixel = origin + delta * coord`
    Vec2 origin;
    float delta;
};
