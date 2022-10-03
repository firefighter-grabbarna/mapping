#pragma once

// Aborts the program immediately.
void panic(const char *msg);
// Asserts that the condition is true, and aborts the program otherwise.
void assert(bool condition, const char *msg);
