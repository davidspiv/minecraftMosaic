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
