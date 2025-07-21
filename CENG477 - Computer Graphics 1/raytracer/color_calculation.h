#ifndef COLOR_CALCULATION_H
#define COLOR_CALCULATION_H

#include "auxilary_functions.h"
#include "intersection.h"

Vec3f color_calculation(const Camera &camera, const Scene &scene, const Intersection &intersection, Ray &ray);

#endif // COLOR_CALCULATION_H
