#include "window.hpp"

#include "../util.hpp"

#include <GLFW/glfw3.h>
#include <cstdlib>
#include <algorithm>

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

void Window::fill(Color color) {
    for (int i = 0; i < this->width * this->height; i++) {
        this->buffer[i * 4 + 0] = 255;
        this->buffer[i * 4 + 1] = color.blue;
        this->buffer[i * 4 + 2] = color.green;
        this->buffer[i * 4 + 3] = color.red;
    }
}

void Window::put(int x, int y, Color color) {
    if (x < 0 || x >= this->width) return;
    if (y < 0 || y >= this->height) return;

    int idx = (x + y * this->width) * 4 + 0;
    this->buffer[idx + 0] = 255;
    this->buffer[idx + 1] = color.blue;
    this->buffer[idx + 2] = color.green;
    this->buffer[idx + 3] = color.red;
}

void Window::line(int x1, int y1, int x2, int y2, Color color) {
    int distance = std::max(abs(x2 - x1), abs(y2 - y1));

    for (int i = 0; i <= distance; i++) {
        float x = x1 + 0.5 + (float) (x2 - x1) / distance * i;
        float y = y1 + 0.5 + (float) (y2 - y1) / distance * i;
        this->put((int) x, (int) y, color);
    }
}

void Window::redraw() {
    glDrawPixels(this->width, this->height, GL_RGBA, GL_UNSIGNED_INT_8_8_8_8, this->buffer);
    glfwSwapBuffers(this->window);
}
