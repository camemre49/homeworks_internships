#include "helper_functions.h"


Vec4 perspectiveDivision(const Vec4 &vtd)
{
    return Vec4{
        vtd.x / vtd.t,
        vtd.y / vtd.t,
        vtd.z / vtd.t,
        1,
        vtd.color };
}

Color colorDifferences(const Color &c1, const Color &c2)
{
    Color temp;
    temp.r = c1.r - c2.r;
    temp.g = c1.g - c2.g;
    temp.b = c1.b - c2.b;
    return temp;
}

Color colorAddition(const Color &c1, const Color &c2)
{
    Color temp;
    temp.r = c1.r + c2.r;
    temp.g = c1.g + c2.g;
    temp.b = c1.b + c2.b;
    return temp;
}

Color colorDivision(const Color &c, int d)
{
    Color temp;
    temp.r = c.r / d;
    temp.g = c.g / d;
    temp.b = c.b / d;
    return temp;
}

Color colorMultiplication(const Color &c, double d)
{
    Color temp;
    temp.r = c.r * d;
    temp.g = c.g * d;
    temp.b = c.b * d;
    return temp;
}

bool visible(double denominator, double numerator, double &tE, double &tL)
{
    double t;
    if (denominator > 0)
    {
        t = numerator / denominator;
        if (t > tL)
        {
            return false;
        }
        if (t > tE)
        {
            tE = t;
        }
    }
    else if (denominator < 0)
    {
        t = numerator / denominator;
        if (t < tE)
        {
            return false;
        }
        if (t < tL)
        {
            tL = t;
        }
    }
    else if (numerator > 0)
    {
        //w is parallel to the edge
        return false;
    }
    return true;
}

bool apply_liang_barsky(Vec4 &p1, Vec4 &p2)
{


    double p1x = p1.x;
    double p1y = p1.y;
    double p1z = p1.z;

    double p2x = p2.x;
    double p2y = p2.y;
    double p2z = p2.z;

    double dx = p2x - p1x;
    double dy = p2y - p1y;
    double dz = p2z - p1z;

    double tE = 0;
    double tL = 1;

    //we are in canonical viewing volume so our min and max values are -1 and 1

    int x_range[2]={-1,1};
    int y_range[2]={-1,1};
    int z_range[2]={-1,1};

    bool isVisible = false;

    Color colorDifference= colorDifferences(p2.color,p1.color);

    if (visible(dx, x_range[0] - p1x, tE, tL) && visible(-dx, p1x - x_range[1], tE, tL) &&
        visible(dy, y_range[0] - p1y, tE, tL) && visible(-dy, p1y - y_range[1], tE, tL) &&
        visible(dz, z_range[0] - p1z, tE, tL) && visible(-dz, p1z - z_range[1], tE, tL))
    {
        if (tL < 1)
        {
            p2.x = p1x + tL * dx;
            p2.y = p1y + tL * dy;
            p2.z = p1z + tL * dz;
            p2.color = colorAddition(p1.color, colorMultiplication(colorDifference, tL));
        }
        if (tE > 0)
        {
            p1.x = p1x + tE * dx;
            p1.y = p1y + tE * dy;
            p1.z = p1z + tE * dz;
            p1.color = colorAddition(p1.color, colorMultiplication(colorDifference, tE));
        }
        isVisible = true;
    }
    return isVisible;

    
}






