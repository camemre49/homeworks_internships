#ifndef __ROTATION_H__
#define __ROTATION_H__
#include <ostream>

#include "Modelling_Transformations.h"

class Rotation: public Modelling_Transformations
{
public:
    int rotationId;
    double angle, ux, uy, uz;

    Rotation();
    Rotation(int rotationId, double angle, double x, double y, double z);
    friend std::ostream &operator<<(std::ostream &os, const Rotation &r);

    Matrix4 getTransformationMatrix() override;
};

#endif