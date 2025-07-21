#ifndef RENDERING_H
#define RENDERING_H
#include "parser.h"

using namespace parser;
void render(unsigned char *image, int startRow, int endRow, int imageWidth, const Camera &camera, const Scene &scene);
#endif // RENDERING_H
