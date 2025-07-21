#ifndef AUXILARY_FUNCTIONS_H
#define AUXILARY_FUNCTIONS_H

#include <cmath>
#include "parser.h"
#include "ray.h"

using namespace parser;

inline float length_of_vector(Vec3f vector)
{
    return sqrt(vector.x*vector.x + vector.y*vector.y + vector.z*vector.z);
}

inline float dot_product(Vec3f vector1, Vec3f vector2)
{
    return vector1.x*vector2.x + vector1.y*vector2.y + vector1.z*vector2.z;
}

inline Vec3f make_unit_vector(Vec3f vector)
{
    float inv_length = 1.0f / length_of_vector(vector);
    return {vector.x * inv_length, vector.y * inv_length, vector.z * inv_length};
}

inline Vec3f cross_product(Vec3f vector1, Vec3f vector2)
{
    return {
        vector1.y * vector2.z - vector1.z * vector2.y, // X component
        vector1.z * vector2.x - vector1.x * vector2.z, // Y component
        vector1.x * vector2.y - vector1.y * vector2.x  // Z component
    };
}

inline float find_distance_between_vectors(Vec3f point1, Vec3f point2)
{
    return sqrt(pow(point1.x - point2.x, 2) + pow(point1.y - point2.y, 2) + pow(point1.z - point2.z, 2));
}

inline float find_determinant_of_matrix_intermsof_vec3f(Vec3f vector1, Vec3f vector2, Vec3f vector3)
{
    return vector1.x*(vector2.y*vector3.z - vector2.z*vector3.y) -
           vector1.y*(vector2.x*vector3.z - vector2.z*vector3.x) +
           vector1.z*(vector2.x*vector3.y - vector2.y*vector3.x);
}

inline Vec3f find_normal_of_triangle(Vec3f point1, Vec3f point2, Vec3f point3)
{
    Vec3f vector1 = {point2.x - point1.x, point2.y - point1.y, point2.z - point1.z};
    Vec3f vector2 = {point3.x - point1.x, point3.y - point1.y, point3.z - point1.z};
    return make_unit_vector(cross_product(vector1, vector2));
}

inline Vec3f vector_multiplication(Vec3f vector, Vec3f vector2)
{
    return {vector.x * vector2.x, vector.y * vector2.y, vector.z * vector2.z};
}

inline Vec3f vector_addition(Vec3f vector, Vec3f vector2)
{
    return {vector.x + vector2.x, vector.y + vector2.y, vector.z + vector2.z};
}

inline Vec3f vector_addition(Vec3f vector, Vec3f vector2, Vec3f vector3)
{
    return {vector.x + vector2.x + vector3.x, vector.y + vector2.y + vector3.y, vector.z + vector2.z + vector3.z};
}

inline Vec3f vector_subtraction(Vec3f vector, Vec3f vector2)
{
    return {vector.x - vector2.x, vector.y - vector2.y, vector.z - vector2.z};
}

inline Vec3f scalar_multiplication(Vec3f vector, float scalar)
{
    return {vector.x * scalar, vector.y * scalar, vector.z * scalar};
}

inline Vec3f scalar_division(Vec3f vector, float scalar)
{
    return {vector.x / scalar, vector.y / scalar, vector.z / scalar};
}

inline Vec3f find_intersection_point(Vec3f ray_origin, Vec3f ray_direction, Vec3f point1, Vec3f point2, Vec3f point3)
{
    Vec3f normal = find_normal_of_triangle(point1, point2, point3);
    float d = -1 * dot_product(normal, point1);
    float t = -(dot_product(normal, ray_origin) + d) / dot_product(normal, ray_direction);

    return vector_addition(ray_origin, scalar_multiplication(ray_direction, t));
}

inline Vec3f calculate_intersection_point(Ray ray, float t)
{
    return vector_addition(ray.origin, scalar_multiplication(ray.direction, t));
}


#endif // AUXILARY_FUNCTIONS_H