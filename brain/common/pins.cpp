#include "pins.hpp"

#include <cstdio>
#include <cstdlib>

void pinMode(int pin, const char *mode) {
    FILE *file = fopen("/sys/class/gpio/export", "w");
    if (file) {
        fprintf(file, "%d", pin);
        fclose(file);
    }

    /*char name[128] = {};
    snprintf(name, sizeof(name), "/sys/class/gpio/gpio%d/direction", pin);

    file = fopen(name, "w");
    if (!file)  {
        std::perror("Setting pin mode failed");
        std::exit(1);
    }
    fprintf(file, "%s", mode);
    fclose(file);*/
}

int pinRead(int pin) {
    char name[128] = {};
    char buf[16] = {};

    snprintf(name, sizeof(name), "/sys/class/gpio/gpio%d/value", pin);
    
    FILE *file = fopen(name, "r");
    if (!file)  {
        std::perror("Reading pin failed");
        std::exit(1);
    }
    size_t len = fread(buf, 1, sizeof(buf) - 1, file);
    buf[len] = 0;
    fclose(file);

    return atoi(buf);
}
