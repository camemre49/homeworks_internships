#ifndef INTERSECTION_H
#define INTERSECTION_H

#include "parser.h"

// Forward declaration of Ray class to avoid circular dependency
class Ray;

enum OBJECT_TYPE {
    TRIANGLE,
    SPHERE,
    MESH
};

/**
 * Class that defines properties of the intersection point of a ray and an object
 */
class Intersection {
public:
    bool is_intersected = false;
    int object_id{};
    OBJECT_TYPE object_type{};
    parser::Vec3f normal{};               // normal vector of the object
    parser::Vec3f intersection_point{};   // coordinates of the intersection point
    int material_id{};                    // material id of the object
    float closest_intersection_distance = -1.0f;                    // distance between the ray's origin and the intersection point

    Intersection(const parser::Scene &scene, const Ray &ray, bool isCullingEnabled);

private:
    void checkIntersectionWithTriangle(const parser::Scene &scene, const Ray &ray, const parser::Triangle &triangle, bool isCullingEnabled);
    void checkIntersectionWithSphere(const parser::Scene &scene, const Ray &ray, const parser::Sphere &sphere);
    void checkIntersectionWithMesh(const parser::Scene &scene, const Ray &ray, const parser::Mesh &mesh, bool isCullingEnabled);
};

#endif // INTERSECTION_H
