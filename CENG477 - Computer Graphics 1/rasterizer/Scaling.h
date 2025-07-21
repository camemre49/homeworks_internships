#ifndef __SCALING_H__
#define __SCALING_H__
#include <ostream>

#include "Modelling_Transformations.h"

class Scaling: public Modelling_Transformations
{
public:
    int scalingId;
    double sx, sy, sz;

    Scaling();
    Scaling(int scalingId, double sx, double sy, double sz);
    friend std::ostream &operator<<(std::ostream &os, const Scaling &s);

    Matrix4 getTransformationMatrix() override;
};

#endif