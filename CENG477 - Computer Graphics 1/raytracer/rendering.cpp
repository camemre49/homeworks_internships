#include "rendering.h"

#include <cmath>
#include "auxilary_functions.h"
#include "color_calculation.h"
#include "intersection.h"
#include "ray.h"
using namespace std;
void render(unsigned char *image, int startRow, int endRow, int imageWidth, const Camera &camera, const Scene &scene)
{
    for (int row = startRow; row < endRow; ++row)
    {
        int index = imageWidth * row * 3;
        for (int column = 0; column < imageWidth; ++column)
        {
            // Create a ray from view point to the current pixel
            Ray created_ray(camera.position, Ray::calculatePixelPosition(camera, row, column));

            // Find the closest valid intersection of the view ray
            Intersection intersection(scene, created_ray, true);

            // Calculate the color of the pixel
            Vec3f calculated_color = color_calculation(camera, scene, intersection, created_ray);

            image[index    ] = static_cast<unsigned char> (calculated_color.x);
            image[index + 1] = static_cast<unsigned char> (calculated_color.y);
            image[index + 2] = static_cast<unsigned char> (calculated_color.z);
            index =  index + 3;
        }
    }
}
