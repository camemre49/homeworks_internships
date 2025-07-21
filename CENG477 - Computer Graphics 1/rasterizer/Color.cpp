#include "Color.h"

#include <cmath>
#include <iomanip>
#include <math.h>

Color::Color() {
    this->r = 0;
    this->g = 0;
    this->b = 0;
}

Color::Color(double r, double g, double b)
{
    this->r = r;
    this->g = g;
    this->b = b;
}

Color::Color(const Color &other)
{
    this->r = other.r;
    this->g = other.g;
    this->b = other.b;
}

std::ostream &operator<<(std::ostream &os, const Color &c)
{
    os << std::fixed << std::setprecision(0) << "rgb(" << c.r << ", " << c.g << ", " << c.b << ")";
    return os;
}

//void Color::roundToInt() {
//    this->r = static_cast<int>(lround(this->r));
//    this->g = static_cast<int>(lround(this->g));
//    this->b = static_cast<int>(lround(this->b));
//}

Color Color::roundToInt() {
    Color newColor;


    newColor. r = static_cast<int>((this->r+0.5));
    newColor. g = static_cast<int>((this->g+0.5));
    newColor. b = static_cast<int>((this->b+0.5));

    return newColor;
}
