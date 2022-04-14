#include <iostream>
#include <filesystem>
#include <optional>
#include <utility>
#include <cmath>

#include "../common/lidar.hpp"
#include "../common/serial.hpp"
#include "../common/util.hpp"

int main(int argc, const char** argv) {
    std::optional<Serial> lidarSerial;
    std::optional<Serial> motorsSerial;
    std::optional<Serial> cannonSerial;

    for (int i = 1; i < argc; i++) {
        std::string entryPath = argv[i];

        std::cout << entryPath << std::endl;

        Serial serial(entryPath.c_str());


        auto response = serial.query("SCIP2.0");
        if (response.empty()) continue;

        if (response[0] == "SCIP2.0") {
            lidarSerial.emplace(std::move(serial));
            std::cout << "Lidar connected" << std::endl;
        } else if (response[0] == "MOTOR") {
            motorsSerial.emplace(std::move(serial));
            std::cout << "Motors connected" << std::endl;
        } else if (response[0] == "CANNON") {
            cannonSerial.emplace(std::move(serial));
            std::cout << "Cannon connected" << std::endl;
        }
    }

    if (!lidarSerial.has_value()) panic("lidar not connected");
    if (!motorsSerial.has_value()) panic("motors not connected");
    //if (!cannonSerial.has_value()) panic("cannon not connected");

    Lidar lidar(std::move(lidarSerial.value()));
    Serial motors(std::move(motorsSerial.value()));
    //Serial cannon(std::move(cannonSerial.value()));

    while (true) {
        std::vector<int> distances = lidar.scan();

        float bestAngle = 0;
        int bestDist = 4000;

        for (size_t i = 0; i < distances.size(); i++) {
            if (distances[i] < 0 || distances[i] > 4000) continue;

            float angle = (float) i / distances.size() * 3.141592 * 2;

            if (distances[i] < bestDist) {
                bestDist = distances[i];
                bestAngle = angle;
            }
        }

        float vx = -sin(bestAngle) * 250.0;
        float vy = -cos(bestAngle) * 250.0;
        float vr = 0.0;

        // Scale down so that the sum doesn't exceed 250
        float scaleFactor = std::min(1.0, 250.0 / (fabs(vx) + fabs(vy) + fabs(vr)));
        vx *= scaleFactor;
        vy *= scaleFactor;
        vr *= scaleFactor;

        char buf[128] = {};
        snprintf(buf, sizeof(buf), "%d %d %d\n", (int) vy, (int) vx, (int) vr);
        motors.output(buf);

        std::cout << bestAngle << std::endl;

    }
}
