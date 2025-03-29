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


ColorXYZ rgbToCIE(const ColorLinRGB &colorLinRGB) {
  // http://www.brucelindbloom.com/index.html?Eqn_RGB_XYZ_Matrix.html
  constexpr std::array<std::array<double, 3>, 3> rgbToCIEMatrix = {
      {{0.4124564, 0.3575761, 0.1804375},
       {0.2126729, 0.7151522, 0.0721750},
       {0.0193339, 0.1191920, 0.9503041}}};

  std::array<double, 3> cieRGB = multiplyMatrix(
      rgbToCIEMatrix, {colorLinRGB.r, colorLinRGB.g, colorLinRGB.b});

  return {cieRGB[0], cieRGB[1], cieRGB[2]};
}


ColorLinRGB cieToRGB(const ColorXYZ &cieColor) {
  constexpr std::array<std::array<double, 3>, 3> cieToRGBMatrix = {{
      {3.2404542, -1.5371385, -0.4985314},
      {-0.9692660, 1.8760108, 0.0415560},
      {0.0556434, -0.2040259, 1.0572252},
  }};

  std::array<double, 3> linRgb =
      multiplyMatrix(cieToRGBMatrix, {cieColor.x, cieColor.y, cieColor.z});

  return {linRgb[0], linRgb[1], linRgb[2]};
}


double distSquared(const Coord &colorA, const Coord &colorB) {
  const double xD = colorB.x - colorA.x;
  const double yD = colorB.y - colorA.y;
  const double zD = colorB.z - colorA.z;
  return xD * xD + yD * yD + zD * zD;
}
