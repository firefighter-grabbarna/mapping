#include "util.hpp"

#include <iostream>

// Aborts the program immediately.
void panic(const char *msg) {
    std::cerr << "error: " << msg << std::endl;
    std::exit(1);
}
// Asserts that the condition is true, and aborts the program otherwise.
void assert(bool condition, const char *msg) {
    if (!condition) panic(msg);
}
