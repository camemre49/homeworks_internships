#ifndef RASTERIZATION_H
#define RASTERIZATION_H
#include "Vec3.h"

struct LineStruct {
    Vec3 startPoint, endPoint;
};

struct TriangleStruct {
    Vec3 v0, v1, v2;
};

struct BycentricCoordinatesStruct {
    double alpha, beta, gamma;
};

void rasterizeLine(LineStruct line);

void rasterizeTriangle(const TriangleStruct& triangle, int sceneXMax, int sceneYMax);


#endif //RASTERIZATION_H
