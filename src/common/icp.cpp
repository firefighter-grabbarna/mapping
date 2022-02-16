#include "icp.hpp"

#include "math.hpp"

#include <cmath>

// Gets the closest point on the walls to the target point.
Point Map::closestPointTo(Point target) const {
    Point closest = target;
    float closestDistSq = INFINITY;

    for (const Line& wall : this->walls) {
        Point point = wall.pointClosestTo(target);
        float distSq = (point - target).magSq();

        if (distSq < closestDistSq) {
            closest = point;
            closestDistSq = distSq;
        }
    }

    return closest;
}

Point Map::raycast(Ray ray) const {
    Point closest = ray.origin;
    float closestDistSq = INFINITY;

    for (const Line& wall : this->walls) {
        auto result = ray.castOnto(wall);
        if (!result.has_value()) continue;

        float distSq = (result.value() - ray.origin).magSq();

        if (distSq < closestDistSq) {
            closest = result.value();
            closestDistSq = distSq;
        }
    }

    return closest;
}

// Performs a single ICP step.
static Transform updateTransformStep(
    Transform oldTransform,
    const Map& map,
    const std::vector<Point> &points
) {
    std::vector<Point> targets = points;
    for (Point &point : targets) {
        point = map.closestPointTo(oldTransform.applyTo(point));
    }

    Vec2 translationSum(0, 0);
    Vec2 positionSum(0, 0);
    for (size_t i = 0; i < points.size(); i++) {
        translationSum = translationSum + (targets[i] - points[i]);
        positionSum = positionSum + targets[i].vec2();
    }
    Vec2 translation = translationSum / points.size();

    Point rotationCenter = (positionSum / points.size()).point();

    // https://en.wikipedia.org/wiki/Procrustes_analysis#Rotation
    float num = 0.0;
    float den = 0.0;

    for (size_t i = 0; i < points.size(); i++) {
        Vec2 p = points[i] + translation - rotationCenter;
        Vec2 t = targets[i] - rotationCenter;

        num += p.cross(t);
        den += p.dot(t);
    }

    Rotation rot1 = atan2(num, den);
    Rotation rot2 = rot1 + 3.1415926535;

    // Compare the two solutions to find which one is the miniumum.
    Transform guess1 = Transform(0, translation - rotationCenter.vec2())
        .rotate(rot1).translate(rotationCenter.vec2());

    Transform guess2 = Transform(0, translation - rotationCenter.vec2())
        .rotate(rot2).translate(rotationCenter.vec2());

    float cost1 = 0.0;
    float cost2 = 0.0;

    for (size_t i = 0; i < points.size(); i++) {
        cost1 += (guess1.applyTo(points[i]) - targets[i]).magSq();
        cost2 += (guess2.applyTo(points[i]) - targets[i]).magSq();
    }

    return cost1 < cost2 ? guess1 : guess2;
}

// Updates the transform by modifying it to align the points to the walls.
//
// Will probably fail on small numbers of points.
Transform updateTransform(
    Transform transform,
    const Map& map,
    const std::vector<Point> &points
) {
    float diffThreshold = 1.0;
    float largestDiff;

    do {
        Transform prevTransform = transform;

        transform = updateTransformStep(prevTransform, map, points);

        largestDiff = 0.0;
        for (const Point &point : points) {
            float diff = (transform.applyTo(point) - prevTransform.applyTo(point)).mag();
            if (diff > largestDiff)
                largestDiff = diff;
        }
    } while (largestDiff > diffThreshold);

    return transform;
}

float transformCost(
    Transform transform,
    const Map& map,
    const std::vector<Point> &points
) {
    float sum = 0.0;

    // To do: Ta hänsyn till hur nära väggarna strålar skulle träffa
    // Kanske bara kolla de ~95% bästa strålarna för att undvika problem med strålar
    // som precis nuddar kanten av en vägg?

    for (const Point &point : points) {
        Point transformed = transform.applyTo(point);
        Point closest = map.closestPointTo(transformed);


        sum += (closest - transformed).magSq();
    }

    //return totalPenetration / (penetrations.size() * 9 / 10);
    return sqrtf(sum / std::max((int) points.size(), 1));
}

// Generates a random float in the specified range.
static float randomFloat(float min, float max) {
    return min + (max - min) / RAND_MAX * rand();
}

Transform searchTransform(const Map& map, const std::vector<Point> &points) {
    float minX = INFINITY, maxX = -INFINITY;
    float minY = INFINITY, maxY = -INFINITY;
    for (const Line &wall : map.walls) {
        minX = std::min(minX, std::min(wall.p1.x, wall.p2.x));
        maxX = std::max(maxX, std::max(wall.p1.x, wall.p2.x));
        minY = std::min(minY, std::min(wall.p1.y, wall.p2.y));
        maxY = std::max(maxY, std::max(wall.p1.y, wall.p2.y));
    }

    Transform best;
    float bestCost = INFINITY;

    for (int i = 0; i < 1000; i++) {
        Transform guess(
            Rotation(randomFloat(-3.14, 3.14)),
            Vec2(randomFloat(minX, maxX), randomFloat(minY, maxY))
        );

        guess = updateTransform(guess, map, points);
        float cost = transformCost(guess, map, points);

        if (cost < bestCost) {
            bestCost = cost;
            best = guess;
        }
    }

    return best;
}
