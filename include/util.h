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

constexpr int blockSize = 16;

ColorRGB getAverageRGB(const Picture &pic, int originX, int originY);

std::vector<ColorRGB> getQuantizedColors();

ColorLinRGB linearize(const ColorRGB &sRGB);

double distSquared(const Coord &colorA, const Coord &colorB);

ColorRGB applyGamma(const ColorLinRGB &linearRGB);

ColorCIELab rgbToCIE(const ColorLinRGB &colorLinRGB);

std::vector<std::vector<int>>
buildLookupTable(const Picture &pic, const std::vector<ColorRGB> &quantColors);

size_t findClosestColorIdx(const ColorRGB &targetColor,
                           const std::vector<ColorRGB> &quantColors);

#endif
