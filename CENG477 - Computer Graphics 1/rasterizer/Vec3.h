#ifndef __VEC3_H__
#define __VEC3_H__
#define NO_COLOR -1
#include <ostream>

#include "Color.h"

class Vec3
{
public:
    double x, y, z;
    int colorId;
    Color color;

    Vec3();
    Vec3(double x, double y, double z);
    Vec3(double x, double y, double z, int colorId);
    Vec3(double x, double y, double z, Color color);
    Vec3(double x, double y, double z, int colorId, Color color);
    Vec3(const Vec3 &other);

    double getNthComponent(int n);

    friend std::ostream &operator<<(std::ostream &os, const Vec3 &v);
};

#endif