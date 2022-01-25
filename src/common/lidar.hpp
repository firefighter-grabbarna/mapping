#include <string>
#include <vector>

class Lidar {
public:
    // Connects to the lidar.
    Lidar(const char *filename);
    // Disconnects from the lidar.
    ~Lidar();
    // Performs a scan. Returns the distances of all angles in counter-clockwise
    // order starting behind the lidar.
    std::vector<int> scan();
private:
    // The file handle.
    int fd;
    // The parameters.
    unsigned dmin, dmax, ares, amin, amax, afrt, rpm;

    // Executes a command and waits for the response.
    std::vector<std::string> query(const char *command);
    // Writes the specified bytes to the port.
    void output(const char *data);
    // Reads a single line from the port. Does not include the newline.
    std::string input();
};
