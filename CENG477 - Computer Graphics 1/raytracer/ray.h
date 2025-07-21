#ifndef RAY_H
#define RAY_H

#include "intersection.h"
#include "parser.h"

class Ray {
public:
    parser::Vec3f origin{};
    parser::Vec3f direction{};
    float direction_square{};
    int recursion_depth{};

    // Parameterized constructor
    Ray(const parser::Vec3f &originPoint, const parser::Vec3f &relatedPixelPosition);

    // Parametrized constructor for shadow rays
    Ray(const parser::Vec3f &intersectionPoint, const parser::Vec3f &lightPosition, float epsilon);

    // Static method to calculate the pixel position 's'
    static parser::Vec3f calculatePixelPosition(const parser::Camera &camera, int row, int column);

    // Method to update the ray after reflection
    void update(const Intersection &intersect, const parser::Scene &scene);
};

#endif // RAY_H
