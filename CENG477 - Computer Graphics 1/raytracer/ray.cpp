#include "ray.h"
#include "auxilary_functions.h"
using namespace parser;

class Intersection;
// Parameterized constructor
Ray::Ray(const Vec3f &originPoint, const Vec3f &relatedPixelPosition) {
    origin = originPoint;
    direction = make_unit_vector(vector_subtraction(relatedPixelPosition, origin));
    direction_square = dot_product(direction, direction);
    recursion_depth = 0;
}

// Parametrized constructor for shadow rays
Ray::Ray(const Vec3f &intersectionPoint, const Vec3f &lightPosition, const float epsilon) {
    direction = make_unit_vector(
               vector_subtraction(lightPosition, intersectionPoint));
    direction_square = dot_product(direction, direction);
    origin = vector_addition(
       intersectionPoint,
       scalar_multiplication(direction, epsilon));
}

// Static method to calculate the pixel position 's'
Vec3f Ray::calculatePixelPosition(const Camera &camera, int row, int column) {
    float shift_in_u = camera.u_factor * (column + 0.5);
    float shift_in_v = camera.v_factor * (row + 0.5);

    // Calculate the desired point on the image plane (pixel position 's')
    Vec3f pixel_position = vector_addition(
        camera.image_top_left,
        vector_subtraction(
            scalar_multiplication(camera.right, shift_in_u),
            scalar_multiplication(camera.up, shift_in_v)));

    return pixel_position;
}

void Ray::update(const Intersection &intersect, const Scene &scene) {
    // Calculate the reflected direction
    Vec3f reflected_direction = scalar_multiplication(
        intersect.normal,
        -2 * dot_product(direction, intersect.normal)
    );
    reflected_direction = make_unit_vector(vector_addition(reflected_direction, direction));

    // Calculate the offset to avoid self intersection
    const Vec3f ref_epsilon = scalar_multiplication(reflected_direction, scene.shadow_ray_epsilon);

    // Update the ray's origin, direction, and other properties
    origin = vector_addition(intersect.intersection_point, ref_epsilon);
    direction = reflected_direction;
    direction_square = dot_product(direction, direction);
    recursion_depth += 1;
}
