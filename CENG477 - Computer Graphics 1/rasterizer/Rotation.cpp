#include <iomanip>
#include "Rotation.h"
#include <cmath>

Rotation::Rotation() {
    this->rotationId = -1;
    this->angle = 0;
    this->ux = 0;
    this->uy = 0;
    this->uz = 0;
}

Rotation::Rotation(int rotationId, double angle, double x, double y, double z)
{
    this->rotationId = rotationId;
    this->angle = angle;
    this->ux = x;
    this->uy = y;
    this->uz = z;
}

std::ostream &operator<<(std::ostream &os, const Rotation &r)
{
    os << std::fixed << std::setprecision(3) << "Rotation " << r.rotationId << " => [angle=" << r.angle << ", " << r.ux << ", " << r.uy << ", " << r.uz << "]";
    return os;
}

Matrix4 Rotation::getTransformationMatrix() {
    double angleInRadians = (angle * M_PI) / 180;
    double rotationMatrix[4][4] = {
        {cos(angleInRadians) + (ux * ux) * (1. - cos(angleInRadians)), ux * uy * (1. - cos(angleInRadians)) - uz * sin(angleInRadians), ux * uz * (1. - cos(angleInRadians)) + uy * sin(angleInRadians), 0},
        {uy * ux * (1. - cos(angleInRadians)) + uz * sin(angleInRadians), cos(angleInRadians) + (uy * uy) * (1. - cos(angleInRadians)), uy * uz * (1. - cos(angleInRadians)) - ux * sin(angleInRadians), 0},
        {uz * ux * (1. - cos(angleInRadians)) - uy * sin(angleInRadians), uz * uy * (1. - cos(angleInRadians)) + ux * sin(angleInRadians), cos(angleInRadians) + (uz * uz) * (1. - cos(angleInRadians)), 0},
        {0, 0, 0, 1}
    };

    return rotationMatrix;
}
