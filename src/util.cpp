#include "../include/util.h"
#include <algorithm>
#include <array>
#include <cmath>


ColorRGB getAverageRGB(const Picture &pic, int originX, int originY) {
  // https://sighack.com/post/averaging-rgb-colors-the-right-way

  double r = 0;
  double b = 0;
  double g = 0;

  int numPx = 0;
  for (int j = originY; j < originY + 16; j++) {
    for (int i = originX; i < originX + 16; i++) {

      const double rComponent = pic.red(i, j);
      const double gComponent = pic.green(i, j);
      const double bComponent = pic.blue(i, j);

      /* Sum the squares of components instead */
      r += rComponent * rComponent;
      g += gComponent * gComponent;
      b += bComponent * bComponent;

      ++numPx;
    }
  }
  /* Return the sqrt of the mean of squared R, G, and B sums */
  return {static_cast<int>(std::round(sqrt(r / numPx))),
          static_cast<int>(std::round(sqrt(g / numPx))),
          static_cast<int>(std::round(sqrt(b / numPx)))};
}


std::vector<ColorRGB> getQuantizedColors() {
  std::vector<ColorRGB> colors;

  for (int i = 0; i <= 255; i += 15) {
    for (int j = 0; j <= 255; j += 15) {
      for (int k = 0; k <= 255; k += 15) {
        colors.push_back({i, j, k});
      }
    }
  }

  return colors;
}


ColorLinRGB linearize(const ColorRGB &sRGB) {
  auto linearizeChannel = [](int c) -> double {
    double normalized = c / 255.0;
    return (normalized <= 0.04045) ? (normalized / 12.92)
                                   : pow((normalized + 0.055) / 1.055, 2.4);
  };

  return {linearizeChannel(sRGB.r), linearizeChannel(sRGB.g),
          linearizeChannel(sRGB.b)};
}


ColorRGB applyGamma(const ColorLinRGB &linearRGB) {

  auto applyGammaToChannel = [](double c) -> int {
    double corrected =
        (c <= 0.0031308) ? (c * 12.92) : 1.055 * pow(c, 1.0 / 2.4) - 0.055;
    return std::clamp(static_cast<int>(corrected * 255.0), 0, 255);
  };

  return {applyGammaToChannel(linearRGB.r), applyGammaToChannel(linearRGB.g),
          applyGammaToChannel(linearRGB.b)};
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


ColorCIELab rgbToCIE(const ColorLinRGB &colorLinRGB) {
  // http://www.brucelindbloom.com/index.html?Eqn_RGB_XYZ_Matrix.html
  constexpr std::array<std::array<double, 3>, 3> rgbToCIEMatrix = {
      {{0.4124564, 0.3575761, 0.1804375},
       {0.2126729, 0.7151522, 0.0721750},
       {0.0193339, 0.1191920, 0.9503041}}};

  std::array<double, 3> cieRGB = multiplyMatrix(
      rgbToCIEMatrix, {colorLinRGB.r, colorLinRGB.g, colorLinRGB.b});

  return {cieRGB[0], cieRGB[1], cieRGB[2]};
}


ColorLinRGB cieToRGB(const ColorCIELab &cieColor) {
  constexpr std::array<std::array<double, 3>, 3> cieToRGBMatrix = {{
      {3.2404542, -1.5371385, -0.4985314},
      {-0.9692660, 1.8760108, 0.0415560},
      {0.0556434, -0.2040259, 1.0572252},
  }};

  std::array<double, 3> linRgb = multiplyMatrix(
      cieToRGBMatrix, {cieColor.lStar, cieColor.aStar, cieColor.bStar});

  return {linRgb[0], linRgb[1], linRgb[2]};
}


double distSquared(const Coord &colorA, const Coord &colorB) {
  const double xD = colorB.x - colorA.x;
  const double yD = colorB.y - colorA.y;
  const double zD = colorB.z - colorA.z;
  return xD * xD + yD * yD + zD * zD;
}


size_t findClosestColorIdx(const ColorRGB &targetColor,
                           const std::vector<ColorRGB> &quantColors) {
  size_t closestColorIdx = 0;
  double minDist = std::numeric_limits<double>::max();

  const ColorCIELab tColorCEI = rgbToCIE(linearize(targetColor));
  const Coord targetCoord = {tColorCEI.lStar, tColorCEI.aStar, tColorCEI.bStar};

  for (size_t i = 0; i < quantColors.size(); i++) {

    const ColorCIELab oColorCEI = rgbToCIE(linearize(quantColors.at(i)));
    const Coord otherCoord = {oColorCEI.lStar, oColorCEI.aStar,
                              oColorCEI.bStar};

    double dist = distSquared(targetCoord, otherCoord);
    if (dist < minDist) {
      minDist = dist;
      closestColorIdx = i;
    }
  }

  return closestColorIdx;
}


std::vector<std::vector<int>>
buildLookupTable(const Picture &pic, const std::vector<ColorRGB> &quantColors) {
  const int cHorizontal = (pic.width() + blockSize - 1) / blockSize;
  const int cVertical = (pic.height() + blockSize - 1) / blockSize;

  std::vector<std::vector<ColorRGB>> avgColors(
      cVertical, std::vector<ColorRGB>(cHorizontal));
  std::vector<std::vector<int>> lookupTable(cVertical,
                                            std::vector<int>(cHorizontal));

  for (int j = 0; j < pic.height(); j += blockSize) {
    for (int i = 0; i < pic.width(); i += blockSize) {

      const ColorRGB avgColor = getAverageRGB(pic, i, j);
      const int newJ = j / blockSize;
      const int newI = i / blockSize;

      avgColors.at(newJ).at(newI) = avgColor;

      const int texIdx = findClosestColorIdx(avgColor, quantColors);
      lookupTable.at(newJ).at(newI) = texIdx;
    }
  }

  return lookupTable;
}
