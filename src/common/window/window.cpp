#include "window.hpp"

#include "../util.hpp"

#include <GLFW/glfw3.h>
#include <cstdlib>

Window::Window(int width, int height, const char *windowName) {
    this->width = width;
    this->height = height;
    this->buffer = new unsigned char[width * height * 4];

    if (!glfwInit()) panic("Could not initialize GLFW");

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    this->window = glfwCreateWindow(width, height, windowName, NULL, NULL);
    if (!window) panic("Could not create a window");

    glfwMakeContextCurrent(this->window);
}

Window::~Window() {
    delete this->buffer;

    glfwDestroyWindow(window);
    glfwTerminate();
}

bool Window::shouldClose() {
    glfwPollEvents();
    return glfwWindowShouldClose(this->window);
}

void Window::fill(
    unsigned char red,
    unsigned char green,
    unsigned char blue
) {
    for (int i = 0; i < this->width * this->height; i++) {
        this->buffer[i * 4 + 0] = 255;
        this->buffer[i * 4 + 1] = blue;
        this->buffer[i * 4 + 2] = green;
        this->buffer[i * 4 + 3] = red;
    }
}

void Window::put(
    int x, int y,
    unsigned char red,
    unsigned char green,
    unsigned char blue
) {
    if (x < 0 || x >= this->width) return;
    if (y < 0 || y >= this->height) return;

    int idx = (x + y * this->width) * 4 + 0;
    this->buffer[idx + 0] = 255;
    this->buffer[idx + 1] = blue;
    this->buffer[idx + 2] = green;
    this->buffer[idx + 3] = red;
}

void Window::line(
    int x1, int y1, int x2, int y2,
    unsigned char red,
    unsigned char green,
    unsigned char blue
) {
    if (abs(x2 - x1) > abs(y2 - y1)) {
        if (x2 < x1) {
            int t = x1;
            x1 = x2;
            x2 = t;
            t = y1;
            y1 = y2;
            y2 = t;
        }

        float y = y1 + 0.5;
        float dy = (float) (y2 - y1) / (x2 - x1);

        for (int x = x1; x <= x2; x++) {
            this->put(x, (int) y, red, green, blue);
            y += dy;
        }
    } else {
        if (y2 < y1) {
            int t = x1;
            x1 = x2;
            x2 = t;
            t = y1;
            y1 = y2;
            y2 = t;
        }

        float x = x1 + 0.5;
        float dx = (float) (x2 - x1) / (y2 - y1);

        for (int y = y1; y <= y2; y++) {
            this->put((int) x, y, red, green, blue);
            x += dx;
        }
    }
}

void Window::redraw() {
    glDrawPixels(this->width, this->height, GL_RGBA, GL_UNSIGNED_INT_8_8_8_8, this->buffer);
    glfwSwapBuffers(this->window);
}
