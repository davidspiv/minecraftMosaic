#include "../include/util.h"
#include "../include/color.h"

#include <algorithm>
#include <array>
#include <cmath>


int distSquared(const StdRGB &colorA, const StdRGB &colorB) {

  const int xD = colorB.r - colorA.r;
  const int yD = colorB.g - colorA.g;
  const int zD = colorB.b - colorA.b;
  return xD * xD + yD * yD + zD * zD;
}


double distSquared(const LinRGB &colorA, const LinRGB &colorB) {

  const double xD = colorB.r - colorA.r;
  const double yD = colorB.g - colorA.g;
  const double zD = colorB.b - colorA.b;
  return xD * xD + yD * yD + zD * zD;
}


double distSquared(const CieLab &colorA, const CieLab &colorB) {

  const double xD = colorB.lStar - colorA.lStar;
  const double yD = colorB.aStar - colorA.aStar;
  const double zD = colorB.bStar - colorA.bStar;
  return xD * xD + yD * yD + zD * zD;
}


std::array<double, 3>
multiplyMatrix(const std::array<std::array<double, 3>, 3> &matrix,
               const std::array<double, 3> &vector) {

  std::array<double, 3> result = {0.0, 0.0, 0.0};

  for (size_t i = 0; i < 3; i++) {
    for (size_t j = 0; j < 3; j++) {
      result[i] += matrix[i][j] * vector[j];
    }
  }

  return result;
}


CieLab getAverage(const Bitmap &bitmap, int originX, int originY) {
  double lStar = 0;
  double aStar = 0;
  double bStar = 0;

  int numPx = 0;

  for (int j = originY; j < std::min(originY + 16, bitmap.height()); j++) {
    for (int i = originX; i < std::min(originX + 16, bitmap.width()); i++) {

      const CieLab cieLabComponent = bitmap.get(i, j);

      /* Sum the squares of components */
      lStar += cieLabComponent.lStar;
      aStar += cieLabComponent.aStar;
      bStar += cieLabComponent.bStar;

      ++numPx;
    }
  }

  if (numPx == 0)
    return CieLab(0, 0, 0); // Prevent division by zero

  lStar /= numPx;
  aStar /= numPx;
  bStar /= numPx;

  return CieLab(lStar, aStar, bStar);
}


size_t findClosestColorIdx(const CieLab &targetColor,
                           const std::vector<CieLab> &quantColors) {
  size_t closestColorIdx = 0;
  double minDist = std::numeric_limits<double>::max();

  for (size_t i = 0; i < quantColors.size(); i++) {

    const CieLab oColorCEI(quantColors.at(i));
    const double curDist = distSquared(targetColor, oColorCEI);

    if (curDist < minDist) {
      minDist = curDist;
      closestColorIdx = i;
    }
  }

  return closestColorIdx;
}


std::vector<std::vector<int>>
buildLookupTable(const Bitmap &bitmap, const std::vector<CieLab> &quantColors) {
  const int cHorizontal = (bitmap.width() + blockSize - 1) / blockSize;
  const int cVertical = (bitmap.height() + blockSize - 1) / blockSize;

  std::vector<std::vector<int>> lookupTable(cVertical,
                                            std::vector<int>(cHorizontal));

  for (int j = 0; j < bitmap.height(); j += blockSize) {
    for (int i = 0; i < bitmap.width(); i += blockSize) {

      const CieLab avgColor = getAverage(bitmap, i, j);
      const int texIdx = findClosestColorIdx(avgColor, quantColors);

      const int newJ = j / blockSize;
      const int newI = i / blockSize;
      lookupTable.at(newJ).at(newI) = texIdx;
    }
  }

  return lookupTable;
}
