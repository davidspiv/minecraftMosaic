#ifndef UTIL_H
#define UTIL_H

#include "../include/color.h"
#include "../include/picture.h"

#include <array>
#include <vector>

constexpr int blockSize = 16;

template <typename CoordT>
double distSquared(const CoordT &coordA, const CoordT &coordB) {
  const double xD = coordB.lStar - coordA.lStar;
  const double yD = coordB.aStar - coordA.aStar;
  const double zD = coordB.bStar - coordA.bStar;
  return xD * xD + yD * yD + zD * zD;
}

std::array<double, 3>
multiplyMatrix(const std::array<std::array<double, 3>, 3> &matrix,
               const std::array<double, 3> &vector);

StdRGB getAverageRGB(const Picture &pic, int originX, int originY);

std::vector<StdRGB> getQuantizedColors();

std::vector<std::vector<int>>
buildLookupTable(const Picture &pic, const std::vector<StdRGB> &quantColors);

size_t findClosestColorIdx(const StdRGB &targetColor,
                           const std::vector<StdRGB> &quantColors);

#endif
