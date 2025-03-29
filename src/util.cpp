#include "../include/util.h"
#include <cmath>

double distSquared(const Color &colorA, const Color &colorB) {
  const double rD = colorB.r - colorA.r;
  const double gD = colorB.g - colorA.g;
  const double bD = colorB.b - colorA.b;
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
