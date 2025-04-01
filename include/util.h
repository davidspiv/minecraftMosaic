#ifndef UTIL_H
#define UTIL_H

#include "../include/color.h"
#include "../include/picture.h"

#include <array>
#include <type_traits>
#include <vector>

constexpr int blockSize = 16;

double distSquared(const LinRGB &colorA, const LinRGB &colorB);
double distSquared(const CieLab &colorA, const CieLab &colorB);

std::array<double, 3>
multiplyMatrix(const std::array<std::array<double, 3>, 3> &matrix,
               const std::array<double, 3> &vector);

StdRGB getAverageRGB(const BitMap &bitMap, int originX, int originY);

std::vector<std::vector<int>>
buildLookupTable(const BitMap &bitMap, const std::vector<StdRGB> &quantColors);

size_t findClosestColorIdx(const StdRGB &targetColor,
                           const std::vector<StdRGB> &quantColors);

#endif
