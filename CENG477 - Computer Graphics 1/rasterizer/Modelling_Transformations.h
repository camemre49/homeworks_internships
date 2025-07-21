#ifndef MODELLING_TRANSFORMATIONS_H
#define MODELLING_TRANSFORMATIONS_H
#include "Matrix4.h"

class Modelling_Transformations {
  protected:
    virtual Matrix4 getTransformationMatrix() = 0;
};

#endif //MODELLING_TRANSFORMATIONS_H
