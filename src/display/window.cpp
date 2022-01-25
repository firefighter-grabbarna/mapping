#include "window.hpp"

#include "../common/util.hpp"

#include <GLFW/glfw3.h>

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

void Window::put(
    int x, int y,
    unsigned char red,
    unsigned char green,
    unsigned char blue
) {
    int idx = (x + y * this->width) * 4 + 0;
    this->buffer[idx + 0] = 255;
    this->buffer[idx + 1] = blue;
    this->buffer[idx + 2] = green;
    this->buffer[idx + 3] = red;
}

void Window::redraw() {
    glDrawPixels(this->width, this->height, GL_RGBA, GL_UNSIGNED_INT_8_8_8_8, this->buffer);
    glfwSwapBuffers(this->window);
}
