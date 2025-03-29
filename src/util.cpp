#include "../include/util.h"
#include <cmath>


// convert sRGB channel to a linear value
double linearizeRGB(int channel) {
  double normalizedRGB = channel / 255.0;

  if (normalizedRGB <= 0.04045) {
    return normalizedRGB / 12.92;
  } else {
    return pow(((normalizedRGB + 0.055) / 1.055), 2.4);
  }
}


int delinearizeRGB(double linearRGB) {
  double sRGB;

  if (linearRGB <= 0.0031308) {
    sRGB = linearRGB * 12.92;
  } else {
    sRGB = 1.055 * pow(linearRGB, 1.0 / 2.4) - 0.055;
  }

  return static_cast<int>(std::round(sRGB * 255.0));
}


double distSquared(const Color &colorA, const Color &colorB) {
  const double rD = linearizeRGB(colorB.r) - linearizeRGB(colorA.r);
  const double gD = linearizeRGB(colorB.g) - linearizeRGB(colorA.g);
  const double bD = linearizeRGB(colorB.b) - linearizeRGB(colorA.b);
  return rD * rD + gD * gD + bD * bD;
}


Color getAverageRGB(const Picture &pic, int originX, int originY) {
  // https://sighack.com/post/averaging-rgb-colors-the-right-way

  double r = 0;
  double b = 0;
  double g = 0;

  int numPx = 0;
  for (int j = originY; j < originY + 16; j++) {
    for (int i = originX; i < originX + 16; i++) {

      const double rComponent = linearizeRGB(pic.red(i, j));
      const double gComponent = linearizeRGB(pic.green(i, j));
      const double bComponent = linearizeRGB(pic.blue(i, j));

      /* Sum the squares of components instead */
      r += rComponent * rComponent;
      g += gComponent * gComponent;
      b += bComponent * bComponent;

      ++numPx;
    }
  }
  /* Return the sqrt of the mean of squared R, G, and B sums */
  return {delinearizeRGB(sqrt(r / numPx)), delinearizeRGB(sqrt(g / numPx)),
          delinearizeRGB(sqrt(b / numPx))};
}
