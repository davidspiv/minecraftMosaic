#ifndef UTIL_H
#define UTIL_H

#include "../include/picture.h"

struct Color {
  int r;
  int g;
  int b;
};

// convert sRGB channel to a linear value
double linearizeRGB(int channel);

int delinearizeRGB(double linearRGB);

double distSquared(const Color &colorA, const Color &colorB);

Color getAverageRGB(const Picture &pic, int originX, int originY);

#endif
