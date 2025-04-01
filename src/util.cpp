#include "../include/util.h"
#include "../include/color.h"
#include "../include/picture.h"
#include "../include/timer.h"

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
  double lStar = 0.0;
  double aStar = 0.0;
  double bStar = 0.0;
  const int maxX = std::min(originX + blockSize, bitmap.width());
  const int maxY = std::min(originY + blockSize, bitmap.height());
  const int numPx = (maxX - originX) * (maxY - originY);

  for (int x = originX; x < maxX; ++x) {
    for (int y = originY; y < maxY; ++y) {
      const CieLab &cieLabComponent = bitmap.get(x, y);

      lStar += cieLabComponent.lStar;
      aStar += cieLabComponent.aStar;
      bStar += cieLabComponent.bStar;
    }
  }

  double invNumPx = 1.0 / numPx;
  return CieLab(lStar * invNumPx, aStar * invNumPx, bStar * invNumPx);
}


size_t findClosestColorIdx(const CieLab &targetColor,
                           const std::vector<CieLab> &quantColors) {
  size_t closestColorIdx = 0;
  double minDist = std::numeric_limits<double>::max();

  for (size_t i = 0; i < quantColors.size(); ++i) {
    const double curDist = distSquared(targetColor, quantColors[i]);

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

  for (int i = 0; i < bitmap.width(); i += blockSize) {
    for (int j = 0; j < bitmap.height(); j += blockSize) {

      const CieLab avgColor = getAverage(bitmap, i, j);
      const int texIdx = findClosestColorIdx(avgColor, quantColors);

      const int newI = i / blockSize;
      const int newJ = j / blockSize;
      lookupTable[newJ][newI] = texIdx;
    }
  }

  return lookupTable;
}


void saveAsPNG(const Bitmap &bitmap) {
  Picture quantPic(bitmap.width(), bitmap.height(), 0, 0, 0);

  for (int i = 0; i < bitmap.width(); i++) {
    for (int j = 0; j < bitmap.height(); j++) {
      auto [r, g, b] = StdRGB(bitmap.get(i, j));

      quantPic.set(i, j, r, g, b);
    }
  }

  quantPic.save("./outputPics/quantizedPic.png");
}
