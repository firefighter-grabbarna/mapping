#pragma once

#include <GLFW/glfw3.h>

struct Color {
    unsigned char red, green, blue;
};

class Window {
public:
    // The size of the window.
    int width, height;

    // Opens a window with the specified dimensions.
    Window(int width, int height, const char *windowName);
    // Closes the window.
    ~Window();

    Window(Window &&window);
    // Polls for events and tests if the window should close.
    bool shouldClose();
    // Fills the window with the specified color.
    void fill(Color color);
    // Changes the color of the specified pixel.
    void put(int x, int y, Color color);
    // Draws a line between the specified points.
    void line(int x1, int y1, int x2, int y2, Color color);
    // Redraws the window.
    void redraw();
private:
    GLFWwindow *window;
    unsigned char *buffer;
};
