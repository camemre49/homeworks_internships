#include "intersection.h"
#include <cmath>
#include <limits>
#include "auxilary_functions.h"
using namespace parser;

Intersection::Intersection(const Scene &scene, const Ray &ray, bool isCullingEnabled) {
    closest_intersection_distance = std::numeric_limits<float>::max();

    // checking intersection with triangles
    for (int i = 0; i < scene.triangles.size(); i++) {
        checkIntersectionWithTriangle(scene, ray, scene.triangles[i], isCullingEnabled);
    }

    for (int i = 0; i < scene.spheres.size(); i++) {
        checkIntersectionWithSphere(scene, ray, scene.spheres[i]);
    }

    for (int i = 0; i < scene.meshes.size(); i++) {
        checkIntersectionWithMesh(scene, ray, scene.meshes[i], isCullingEnabled);
    }
}

void Intersection::checkIntersectionWithTriangle(const Scene &scene, const Ray &ray, const Triangle &triangle, bool isCullingEnabled) {
    Vec3f point1 = scene.vertex_data[triangle.indices.v0_id - 1];
    Vec3f point2 = scene.vertex_data[triangle.indices.v1_id - 1];
    Vec3f point3 = scene.vertex_data[triangle.indices.v2_id - 1];

    Vec3f first_vector = vector_subtraction(point1, point2);
    Vec3f second_vector = vector_subtraction(point1, point3);
    Vec3f third_vector = vector_subtraction(point1, ray.origin);

    if (isCullingEnabled) {
        Vec3f normal_of_triangle = make_unit_vector(cross_product(first_vector, second_vector));
        if (dot_product(normal_of_triangle, ray.direction) > 0) return;  // ignore triangle
    }

    float determinant_of_a = find_determinant_of_matrix_intermsof_vec3f(first_vector, second_vector, ray.direction);
    if (determinant_of_a == 0) return;  // no solution

    float t = find_determinant_of_matrix_intermsof_vec3f(first_vector, second_vector, third_vector) / determinant_of_a;
    if (t <= 0) return;

    float gamma = find_determinant_of_matrix_intermsof_vec3f(first_vector, third_vector, ray.direction) / determinant_of_a;
    if (gamma < 0 || gamma > 1) return;

    float beta = find_determinant_of_matrix_intermsof_vec3f(third_vector, second_vector, ray.direction) / determinant_of_a;
    if (beta < 0 || beta > (1 - gamma)) return;

    if (t < closest_intersection_distance) {
        is_intersected = true;
        closest_intersection_distance = t;
        material_id = triangle.material_id;
        normal = find_normal_of_triangle(point1, point2, point3);
        intersection_point = calculate_intersection_point(ray, t);
        object_type = TRIANGLE;
        object_id = triangle.indices.v0_id;
    }
}

void Intersection::checkIntersectionWithSphere(const Scene &scene, const Ray &ray, const Sphere &sphere) {
    const Vec3f center_of_circle = scene.vertex_data[sphere.center_vertex_id - 1];
    const Vec3f origin_to_center = vector_subtraction(ray.origin, center_of_circle);

    float a = ray.direction_square;
    float b = 2 * dot_product(ray.direction, origin_to_center);
    float c = dot_product(origin_to_center, origin_to_center) - sphere.radius * sphere.radius;

    float discriminant = b * b - 4 * a * c;
    if (discriminant < 0) return;  // no solution

    float discriminant_s = std::sqrt(discriminant);
    float two_a = 2 * a;
    float min_solution = std::min(
        (-b + discriminant_s) / two_a,
        (-b - discriminant_s) / two_a
    );

    // No need to check other solution because it is behind
    if (min_solution > 0 && min_solution < closest_intersection_distance) {
        is_intersected = true;
        object_id = sphere.center_vertex_id;
        object_type = SPHERE;
        material_id = sphere.material_id;
        closest_intersection_distance = min_solution;
        intersection_point = calculate_intersection_point(ray, min_solution);
        normal = make_unit_vector(vector_subtraction(intersection_point, center_of_circle));
    }
}

void Intersection::checkIntersectionWithMesh(const Scene &scene, const Ray &ray, const Mesh &mesh, bool isCullingEnabled) {
    for (int i = 0; i < mesh.faces.size(); i++) {
        Triangle triangle = {mesh.material_id, mesh.faces[i]};
        checkIntersectionWithTriangle(scene, ray, triangle, isCullingEnabled);
    }
}
