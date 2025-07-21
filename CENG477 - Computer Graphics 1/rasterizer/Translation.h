#ifndef __TRANSLATION_H__
#define __TRANSLATION_H__
#include <ostream>

#include "Modelling_Transformations.h"

class Translation: public Modelling_Transformations
{
public:
    int translationId;
    double tx, ty, tz;

    Translation();
    Translation(int translationId, double tx, double ty, double tz);
    friend std::ostream &operator<<(std::ostream &os, const Translation &t);

    Matrix4 getTransformationMatrix() override;
};

#endif