#ifndef UTIL_H
#define UTIL_H

#include "../include/picture.h"

struct ColorRGB {
  int r;
  int g;
  int b;
};

struct ColorCIELab {
  double lStar, aStar, bStar;
};

struct ColorLinRGB {
  double r, g, b;
};

struct Coord {
  double x, y, z;
};

ColorLinRGB linearize(const ColorRGB &sRGB);

double distSquared(const Coord &colorA, const Coord &colorB);

ColorRGB getAverageRGB(const Picture &pic, int originX, int originY);

ColorRGB applyGamma(const ColorLinRGB &linearRGB);

ColorCIELab rgbToCIE(const ColorLinRGB &colorLinRGB);

#endif
