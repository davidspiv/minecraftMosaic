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

// NONE
// StdRGB getAverageRGB(const Picture &pic, int originX, int originY) {
//   // https://sighack.com/post/averaging-rgb-colors-the-right-way
//   double r = 0;
//   double g = 0;
//   double b = 0;

//   int numPx = 0;

//   for (int j = originY; j < std::min(originY + 16, pic.height()); j++) {
//     for (int i = originX; i < std::min(originX + 16, pic.width()); i++) {

//       /* Sum the squares of components */
//       r += pic.red(i, j);
//       g += pic.green(i, j);
//       b += pic.blue(i, j);

//       ++numPx;
//     }
//   }

//   if (numPx == 0)
//     return StdRGB(0, 0, 0); // Prevent division by zero

//   r /= numPx;
//   g /= numPx;
//   b /= numPx;

//   return StdRGB(int(std::round(r)), int(std::round(g)), int(std::round(b)));
// }

// // LINEAR
// StdRGB getAverageRGB(const Picture &pic, int originX, int originY) {
//   // https://sighack.com/post/averaging-rgb-colors-the-right-way
//   double r = 0;
//   double g = 0;
//   double b = 0;

//   int numPx = 0;

//   for (int j = originY; j < std::min(originY + 16, pic.height()); j++) {
//     for (int i = originX; i < std::min(originX + 16, pic.width()); i++) {
//       const StdRGB stdRGB(pic.red(i, j), pic.green(i, j), pic.blue(i, j));
//       const LinRGB linRGBComponent(stdRGB);

//       /* Sum the squares of components */
//       r += linRGBComponent.r;
//       g += linRGBComponent.g;
//       b += linRGBComponent.b;

//       ++numPx;
//     }
//   }

//   if (numPx == 0)
//     return StdRGB(0, 0, 0); // Prevent division by zero

//   r /= numPx;
//   g /= numPx;
//   b /= numPx;

//   const LinRGB linRGBFinal(r, g, b);
//   return StdRGB(linRGBFinal);
// }


// LAB
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
      lStar += cieLabComponent.lStar;
      aStar += cieLabComponent.aStar;
      bStar += cieLabComponent.bStar;

      ++numPx;
    }
  }

  if (numPx == 0)
    return StdRGB(0, 0, 0); // Prevent division by zero

  lStar /= numPx;
  aStar /= numPx;
  bStar /= numPx;

  const CieLab cieLabFinal(lStar, aStar, bStar);
  return StdRGB(cieLabFinal);
}


// NONE
// size_t findClosestColorIdx(const StdRGB &targetColor,
//                            const std::vector<StdRGB> &quantColors) {
//   size_t closestColorIdx = 0;
//   int minDist = std::numeric_limits<int>::max();

//   for (size_t i = 0; i < quantColors.size(); i++) {

//     const LinRGB oColorLin(quantColors.at(i));
//     int dist = distSquared(targetColor, quantColors.at(i));

//     if (dist < minDist) {
//       minDist = dist;
//       closestColorIdx = i;
//     }
//   }

//   return closestColorIdx;
// }


// LINEAR
// size_t findClosestColorIdx(const StdRGB &targetColor,
//                            const std::vector<StdRGB> &quantColors) {
//   size_t closestColorIdx = 0;
//   double minDist = std::numeric_limits<double>::max();

//   const LinRGB tColorLin(targetColor);

//   for (size_t i = 0; i < quantColors.size(); i++) {

//     const LinRGB oColorLin(quantColors.at(i));
//     double dist = distSquared(tColorLin, oColorLin);

//     if (dist < minDist) {
//       minDist = dist;
//       closestColorIdx = i;
//     }
//   }

//   return closestColorIdx;
// }


// LAB
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

  std::vector<std::vector<int>> lookupTable(cVertical,
                                            std::vector<int>(cHorizontal));

  for (int j = 0; j < pic.height(); j += blockSize) {
    for (int i = 0; i < pic.width(); i += blockSize) {

      const StdRGB avgColor = getAverageRGB(pic, i, j);
      const int texIdx = findClosestColorIdx(avgColor, quantColors);

      const int newJ = j / blockSize;
      const int newI = i / blockSize;
      lookupTable.at(newJ).at(newI) = texIdx;
    }
  }

  return lookupTable;
}
