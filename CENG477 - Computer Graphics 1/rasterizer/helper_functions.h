#ifndef __HELPERFUNCTIONS_H__
#define __HELPERFUNCTIONS_H__

#include "Vec4.h"

Vec4 perspectiveDivision(const Vec4 & vtd);
Color colorDifferences(const Color &c1, const Color &c2);
Color colorDivision(const Color &c, int d);
Color colorAddition(const Color &c1, const Color &c2);
Color colorMultiplication(const Color &c, double d);
bool visible(double denominator, double numerator, double &tE, double &tL);
bool apply_liang_barsky(Vec4 &p1, Vec4 &p2);


#endif