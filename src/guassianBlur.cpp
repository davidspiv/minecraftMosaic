#include <algorithm>
#include <cmath>
#include <vector>

#include "../include/picture.h"

int mirrorPixel(int x, int max) {
  if (x < 0)
    return -x; // Mirror left/top side
  if (x >= max)
    return 2 * (max - 1) - x; // Mirror right/bottom side
  return x;
}

// creates a 1D matrix that, when multiplied by its transposed counterpart
// will create a normalized gaussian kernel of arbitrary size
std::vector<double> calcGaussianKernelComponent(size_t size) {
  double sigma = (size - 1) / 6.0;

  std::vector<double> kernel(size, 0);
  int radius = size / 2;
  double sum = 0.0;

  // Compute the raw Gaussian values
  for (int i = -radius; i <= radius; i++) {
    kernel[i + radius] = round(exp(-(i * i) / (2.0 * sigma * sigma)) * 100);
    sum += kernel[i + radius];
  }

  // Normalize the kernel
  transform(kernel.begin(), kernel.end(), kernel.begin(),
            [sum](double val) { return (val / sum); });

  return kernel;
}


void gaussianBlur(Picture &pic, const size_t strength) {
  Picture tempPic = pic;
  const size_t width = pic.width();
  const size_t height = pic.height();

  // kSize will be rounded down to an odd number to keep target pixel centered
  const size_t kSize = strength % 2 ? strength : strength - 1;

  // half the matrix not including the center
  const int kRadius = kSize / 2;

  // used both as an 1 x kSize kernel and a transposed kSize x 1 kernel
  std::vector<double> gaussianKernelComponent =
      calcGaussianKernelComponent(kSize);

  //  horizontal first pass writes to temporary picture object
  for (size_t j = 0; j < height; j++) {
    for (size_t i = 0; i < width; i++) {
      double rWeightedAvg = 0;
      double gWeightedAvg = 0;
      double bWeightedAvg = 0;

      for (int k = -kRadius; k <= kRadius; k++) {
        const double weight = gaussianKernelComponent[k + kRadius];
        const size_t pixel = mirrorPixel(i + k, width);

        rWeightedAvg += weight * pic.red(pixel, j);
        gWeightedAvg += weight * pic.green(pixel, j);
        bWeightedAvg += weight * pic.blue(pixel, j);
      }

      tempPic.set(i, j, rWeightedAvg, gWeightedAvg, bWeightedAvg, 255);
    }
  }

  // vertical second pass writes directly to final picture object
  for (size_t j = 0; j < height; j++) {
    for (size_t i = 0; i < width; i++) {
      double rWeightedAvg = 0;
      double gWeightedAvg = 0;
      double bWeightedAvg = 0;

      for (int k = -kRadius; k <= kRadius; k++) {
        const double weight = gaussianKernelComponent[k + kRadius];
        const size_t pixel = mirrorPixel(j + k, height);

        rWeightedAvg += weight * tempPic.red(i, pixel);
        gWeightedAvg += weight * tempPic.green(i, pixel);
        bWeightedAvg += weight * tempPic.blue(i, pixel);
      }

      pic.set(i, j, rWeightedAvg, gWeightedAvg, bWeightedAvg, 255);
    }
  }
}
