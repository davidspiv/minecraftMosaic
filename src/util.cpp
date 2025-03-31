#include "../include/util.h"
#include "../include/color.h"

#include <algorithm>
#include <array>
#include <cmath>


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


StdRGB getAverageRGB(const Picture &pic, int originX, int originY) {
  // https://sighack.com/post/averaging-rgb-colors-the-right-way
  double lStar = 0;
  double aStar = 0;
  double bStar = 0;

  int numPx = 0;

  for (int j = originY; j < std::min(originY + 16, pic.height()); j++) {
    for (int i = originX; i < std::min(originX + 16, pic.width()); i++) {
      const StdRGB stdRGB(pic.red(i, j), pic.green(i, j), pic.blue(i, j));
      const CieLab cieLabComponent(stdRGB);

      /* Sum the squares of components */
      lStar += cieLabComponent.lStar * cieLabComponent.lStar;
      aStar += cieLabComponent.aStar * cieLabComponent.aStar;
      bStar += cieLabComponent.bStar * cieLabComponent.bStar;

      ++numPx;
    }
  }

  if (numPx == 0)
    return StdRGB(0, 0, 0); // Prevent division by zero

  /* Compute the root mean square */
  lStar = std::sqrt(lStar / numPx);
  aStar = std::sqrt(aStar / numPx);
  bStar = std::sqrt(bStar / numPx);

  const CieLab cieLabFinal(lStar, aStar, bStar);
  return StdRGB(cieLabFinal);
}


std::vector<StdRGB> getQuantizedColors() {
  std::vector<StdRGB> colors;

  for (int i = 0; i <= 255; i += 15) {
    for (int j = 0; j <= 255; j += 15) {
      for (int k = 0; k <= 255; k += 15) {
        colors.push_back({i, j, k});
      }
    }
  }

  return colors;
}


size_t findClosestColorIdx(const StdRGB &targetColor,
                           const std::vector<StdRGB> &quantColors) {
  size_t closestColorIdx = 0;
  double minDist = std::numeric_limits<double>::max();

  const CieLab tColorCEI(targetColor);

  for (size_t i = 0; i < quantColors.size(); i++) {

    const CieLab oColorCEI(quantColors.at(i));

    double dist = distSquared(tColorCEI, oColorCEI);
    if (dist < minDist) {
      minDist = dist;
      closestColorIdx = i;
    }
  }

  return closestColorIdx;
}


std::vector<std::vector<int>>
buildLookupTable(const Picture &pic, const std::vector<StdRGB> &quantColors) {
  const int cHorizontal = (pic.width() + blockSize - 1) / blockSize;
  const int cVertical = (pic.height() + blockSize - 1) / blockSize;

  std::vector<std::vector<StdRGB>> avgColors(cVertical,
                                             std::vector<StdRGB>(cHorizontal));
  std::vector<std::vector<int>> lookupTable(cVertical,
                                            std::vector<int>(cHorizontal));

  for (int j = 0; j < pic.height(); j += blockSize) {
    for (int i = 0; i < pic.width(); i += blockSize) {

      const StdRGB avgColor = getAverageRGB(pic, i, j);
      const int newJ = j / blockSize;
      const int newI = i / blockSize;

      avgColors.at(newJ).at(newI) = avgColor;

      const int texIdx = findClosestColorIdx(avgColor, quantColors);
      lookupTable.at(newJ).at(newI) = texIdx;
    }
  }

  return lookupTable;
}
