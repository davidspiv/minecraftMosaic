#ifndef UTIL_H
#define UTIL_H

#include "../include/picture.h"

struct Color {
  int r;
  int g;
  int b;
};

double distSquared(const Color &colorA, const Color &colorB);

Color getAverageRGB(const Picture &pic, int originX, int originY);

#endif
