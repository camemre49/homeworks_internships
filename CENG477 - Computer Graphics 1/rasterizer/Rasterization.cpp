#include "Rasterization.h"
#include "helper_functions.h"
#include "Scene.h"
#include<bits/stdc++.h>

void rasterizeLine(LineStruct line)
{
    double dx = line.endPoint.x - line.startPoint.x;
    double dy = line.endPoint.y - line.startPoint.y;
    double dz = line.endPoint.z - line.startPoint.z;

    //double original_depth_difference = line.endPoint.z - line.startPoint.z;

    // Represent positive or negative direction (it may be horizontal or vertical)
    int moveDirection;
    // |m| > 1


    // LINE RASTERIZATION PARALLELER YOK, ONLARI ÇİZMİYOR

    if(std::abs(dy) > std::abs(dx)) {
        if (dy < 0)
        {
            std::swap(line.endPoint, line.startPoint);
            dx= line.endPoint.x - line.startPoint.x;
            dz = line.endPoint.z - line.startPoint.z;
            dy = line.endPoint.y - line.startPoint.y;
        }

        moveDirection = (dx < 0) ? -1 : 1;
        double d = (-dx) + (moveDirection * 0.5 * (-dy));

        Color colorDifference = colorDifferences(line.endPoint.color, line.startPoint.color);
        Color colorStep = colorDivision(colorDifference, static_cast<int>(dy));
        
        
        
        
        Color currentColor = (line.startPoint.color);
        

        double depthStep = dz / std::abs(dy);
        double currentDepth = line.startPoint.z;

        int currentX = static_cast<int>(line.startPoint.x);

        for (int i = static_cast<int>(line.startPoint.y); i <= line.endPoint.y; i++)
        {
            //if(i<0 || i>=Scene::image[0].size() || currentX<0 || currentX>=Scene::image.size()) continue;

            Color rounded_color=  currentColor.roundToInt();
            // Set the z buffer value
            if (currentDepth < Scene::zBuffer[currentX][i]) {
                Scene::zBuffer[currentX][i] = currentDepth;
                Scene::image[currentX][i] = rounded_color;
            }
            currentDepth += depthStep;

            // Set the color value

            currentColor = colorAddition(currentColor, colorStep);

            if (d * moveDirection <= 0)
            {
                d += dx;
            }
            else
            {
                d += dx + (moveDirection * -dy);
                currentX += moveDirection;
            }
        }
    }
    else 
    { 
        // |m| =< 1
        if (line.endPoint.x < line.startPoint.x)
        {
            std::swap(line.endPoint, line.startPoint);
            dx= line.endPoint.x - line.startPoint.x;
            dz = line.endPoint.z - line.startPoint.z;
            dy = line.endPoint.y - line.startPoint.y;
        }

        moveDirection = (dy < 0) ? -1 : 1;
        double depthStep = dz / std::abs(dx);
        double currentDepth = line.startPoint.z;

        double d = (line.startPoint.y - line.endPoint.y) + (moveDirection * 0.5 * (line.endPoint.x - line.startPoint.x));

        Color colorDifference = colorDifferences(line.endPoint.color, line.startPoint.color);
        Color colorStep = colorDivision(colorDifference, static_cast<int>(dx));
        Color currentColor = (line.startPoint.color);
        

        int currentY = static_cast<int>(line.startPoint.y);


        
        for (int i = static_cast<int>(line.startPoint.x); i <= line.endPoint.x; i++)
        {
            //if(i<0 || i>=Scene::image.size() || currentY<0 || currentY>=Scene::image[0].size()) continue;

            Color rounded_color=  currentColor.roundToInt();
            if (currentDepth < Scene::zBuffer[i][currentY]) {
                Scene::zBuffer[i][currentY] = currentDepth;
                Scene::image[i][currentY] = rounded_color;
            }

            currentDepth += depthStep;


            if (d * moveDirection >= 0)
            {
                d += (line.startPoint.y - line.endPoint.y); // move horizontally
            }
            else
            {
                d += (line.startPoint.y - line.endPoint.y) + (moveDirection * (line.endPoint.x - line.startPoint.x));
                currentY += moveDirection;
            }
            currentColor = colorAddition(currentColor, colorStep);
        }
    }
}

Color triangleBycentricColorInterpolation(const BycentricCoordinatesStruct& barycentric, const TriangleStruct& triangle);
BycentricCoordinatesStruct calculateBarycentricCoordinates(const TriangleStruct& triangle, double x, double y);
void rasterizeTriangle(const TriangleStruct& triangle, const int sceneXMax, const int sceneYMax)
{
	// Get the bounding box of the triangle
	double x_min = std::max(0.0, std::min({triangle.v0.x, triangle.v1.x, triangle.v2.x}));
	double x_max = std::min(sceneXMax - 1.0, std::max({triangle.v0.x, triangle.v1.x, triangle.v2.x}));
	double y_min = std::max(0.0, std::min({triangle.v0.y, triangle.v1.y, triangle.v2.y}));
	double y_max = std::min(sceneYMax - 1.0, std::max({triangle.v0.y, triangle.v1.y, triangle.v2.y}));


    // Iterate through all pixels in the bounding box
    for (int y = static_cast<int>(y_min); y <= static_cast<int>(y_max); y++)
    {
        for (int x = static_cast<int>(x_min); x <= static_cast<int>(x_max); x++)
        {
            // Calculate barycentric coordinates
            BycentricCoordinatesStruct bCoords = calculateBarycentricCoordinates(triangle, x, y);

            // Check if the point (x, y) is inside the triangle
            if (bCoords.alpha >= 0 && bCoords.beta >= 0 && bCoords.gamma >= 0)
            {
                // Compute the interpolated depth (z) at the pixel
                double pixelDepth = bCoords.alpha * triangle.v0.z + bCoords.beta * triangle.v1.z + bCoords.gamma * triangle.v2.z;
                if (pixelDepth < Scene::zBuffer[x][y])
                {
                    Scene::zBuffer[x][y] = pixelDepth;
                    // Interpolate and round the color values and store it in the image
                    Scene::image[x][y] = triangleBycentricColorInterpolation(bCoords, triangle);
                }
            }
        }
    }
}

Color triangleBycentricColorInterpolation(const BycentricCoordinatesStruct& barycentric, const TriangleStruct& triangle) {
    // Interpolate the color based on barycentric coordinates
    Color color(
        barycentric.alpha * triangle.v0.color.r + barycentric.beta * triangle.v1.color.r + barycentric.gamma * triangle.v2.color.r,
        barycentric.alpha * triangle.v0.color.g + barycentric.beta * triangle.v1.color.g + barycentric.gamma * triangle.v2.color.g,
        barycentric.alpha * triangle.v0.color.b + barycentric.beta * triangle.v1.color.b + barycentric.gamma * triangle.v2.color.b
    );
    color.roundToInt();
    return color;
}

BycentricCoordinatesStruct calculateBarycentricCoordinates(const TriangleStruct& triangle, double x, double y) {
    // Compute the area of the whole triangle (v0, v1, v2)
    double area = (triangle.v0.x * (triangle.v1.y - triangle.v2.y) +
                         triangle.v1.x * (triangle.v2.y - triangle.v0.y) +
                         triangle.v2.x * (triangle.v0.y - triangle.v1.y));

    // Compute the areas of the sub-triangles (v1, v2, P), (v0, v2, P), (v0, v1, P)
    double area_alpha = (x * (triangle.v1.y - triangle.v2.y) +
                               triangle.v1.x * (triangle.v2.y - y) +
                               triangle.v2.x * (y - triangle.v1.y));

    double area_beta = (triangle.v0.x * (y - triangle.v2.y) +
                              x * (triangle.v2.y - triangle.v0.y) +
                              triangle.v2.x * (triangle.v0.y - y));

    double area_gamma =(triangle.v0.x * (triangle.v1.y - y) +
                               triangle.v1.x * (y - triangle.v0.y) +
                               x * (triangle.v0.y - triangle.v1.y));

    // Calculate alpha, beta, and gamma
    BycentricCoordinatesStruct coords;
    coords.alpha = area_alpha / area;
    coords.beta = area_beta / area;
    coords.gamma = area_gamma / area;

    return coords;
}

