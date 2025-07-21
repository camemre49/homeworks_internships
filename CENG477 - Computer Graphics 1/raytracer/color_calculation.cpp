#include "color_calculation.h"
#include <cmath>
#include "auxilary_functions.h"
#include "parser.h"

using namespace std;
using namespace parser;

Vec3f calculate_ambient_shading(const Scene &scene, int material_id);
Vec3f calculate_diffuse_shading(const PointLight &light, const Intersection &inter, const Material &material);
Vec3f calculate_specular_shading(const PointLight &light, const Intersection &inter, const Material &material, const Ray &ray);

Vec3f color_calculation(const Camera &camera, const Scene &scene, const Intersection &intersection, Ray &ray)
{
    // If no intersection occurred, return background color
    if (!intersection.is_intersected) {

        if(ray.recursion_depth > 0) {
            return Vec3f{0.0f, 0.0f, 0.0f};
        }


        return Vec3f{
            static_cast<float>(scene.background_color.x),
            static_cast<float>(scene.background_color.y),
            static_cast<float>(scene.background_color.z)
        };
    }

    // Get the material
    const Material material = scene.materials[intersection.material_id- 1];

    // Start with the ambient color
    Vec3f color = vector_multiplication(material.ambient, scene.ambient_light);

    size_t light_count = scene.point_lights.size();
    for (int i = 0; i < light_count; i++)
    {
        Ray shadowRay(intersection.intersection_point, scene.point_lights[i].position, scene.shadow_ray_epsilon);
        Intersection shadowIntersection(scene, shadowRay, false);

        if (shadowIntersection.is_intersected)
        {
            float distance_between_light_and_intersection =
                length_of_vector(
                    vector_subtraction(
                        scene.point_lights[i].position,
                        intersection.intersection_point));
            // If intersection is beyond the light, no shadow, add the colors
            if (!(shadowIntersection.closest_intersection_distance < distance_between_light_and_intersection))
            {
                color = vector_addition(
                color,
                calculate_diffuse_shading(scene.point_lights[i], intersection, material),
                calculate_specular_shading(scene.point_lights[i], intersection, material, ray)
                );
            }
        } else {
            // No intersection means no shadow, add the colors
            color = vector_addition(
                color,
                calculate_diffuse_shading(scene.point_lights[i], intersection, material),
                calculate_specular_shading(scene.point_lights[i], intersection, material, ray)
                );
        }
    }

    // Calculate the reflection
    Vec3f reflectedColor = {0, 0, 0};
    bool isMirror = material.mirror.x > 0 || material.mirror.y > 0 || material.mirror.z > 0;

    if (isMirror && ray.recursion_depth < scene.max_recursion_depth)
    {
        // Calculate the reflection ray
        ray.update(intersection, scene);
        Intersection reflected_intersection(scene, ray, false);

        if (reflected_intersection.object_id != intersection.object_id)
        {
            reflectedColor = color_calculation(camera, scene, reflected_intersection, ray);
        }

        color = vector_addition(
            color,
            vector_multiplication(reflectedColor, material.mirror));
    }

    return {
        min(255.0f, color.x),
        min(255.0f, color.y),
        min(255.0f, color.z),};
}


// Not used for efficiency
Vec3f calculate_ambient_shading(const Scene &scene, int material_id)
{
    Vec3f ambientShading = vector_multiplication(scene.materials[material_id].ambient, scene.ambient_light);
    return ambientShading;
}

Vec3f calculate_diffuse_shading(const PointLight &light, const Intersection &inter, const Material &material)
{
    // Compute the light direction vector and its length
    Vec3f light_direction = vector_subtraction(light.position, inter.intersection_point);
    float light_length_squared = dot_product(light_direction, light_direction); // Equivalent to length^2 without sqrt

    // Compute the unit vector of light direction and the cosine of the angle
    Vec3f unit_light_direction = make_unit_vector(light_direction);
    float cosine = max(0.0f, dot_product(unit_light_direction, inter.normal));

    // Calculate the radiance in a single step
    Vec3f radiance = scalar_multiplication(vector_multiplication(light.intensity, material.diffuse), cosine);

    // Divide by the squared length
    radiance = scalar_division(radiance, light_length_squared);

    return radiance;
}


Vec3f calculate_specular_shading(const PointLight &light, const Intersection &intersection, const Material &material, const Ray &ray)
{

    Vec3f light_vector = vector_subtraction(light.position, intersection.intersection_point);
    Vec3f light_direction = make_unit_vector(light_vector);
    if(dot_product(light_direction, intersection.normal) < 0.0f) {
        return Vec3f{0, 0, 0};
    }

    float light_vector_length_squared = dot_product(light_vector, light_vector);
    Vec3f received_radiance= scalar_division(light.intensity, light_vector_length_squared);

    Vec3f half_vector = make_unit_vector(vector_subtraction(light_direction, ray.direction));
    float angle_between_normal_and_half_vector = max(0.0f, dot_product(intersection.normal, half_vector));

    // Calculate and return the radiance
    return vector_multiplication(received_radiance, scalar_multiplication(material.specular, pow(angle_between_normal_and_half_vector, material.phong_exponent)));
;
}


