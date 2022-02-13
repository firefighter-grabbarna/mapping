#pragma once

#include "math.hpp"

#include <vector>

// A map of the area
class Map {
public:
    std::vector<Line> walls;

    Map(const std::vector<Line> &walls) : walls(walls) {}
    Map(std::vector<Line> &&walls) : walls(std::move(walls)) {}

    // Gets the closest point on the walls to the target point.
    Point closestPointTo(Point target) const;
};

// Updates the transform by modifying it to align the points to the walls of the map.
Transform updateTransform(Transform transform, const Map& map, const std::vector<Point> &points);

// A cost function for determining the accuracy of the transform
float transformCost(Transform transform, const Map& map, const std::vector<Point> &points);
