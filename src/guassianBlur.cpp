#include <algorithm>
#include <cmath>
#include <vector>

#include "../include/Timer.h"
#include "../include/picture.h"
#include "../include/util.h"

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


void Picture::gaussianBlur(const size_t strength) {
  Timer timer("Gaussian Blur");
  Picture tempPic = *this;
  const size_t width = _width;
  const size_t height = _height;

  // kSize will be rounded down to an odd number to keep target pixel centered
  const size_t kSize = strength % 2 ? strength : strength - 1;

  // half the matrix not including the center
  const int kRadius = kSize / 2;

  // used both as an 1 x kSize kernel and a transposed kSize x 1 kernel
  std::vector<double> gaussianKernelComponent =
      calcGaussianKernelComponent(kSize);

  //  horizontal first pass writes to temporary picture object
  process2dInParallel(height, width, [&](int i, int j) {
    double rWeightedAvg = 0;
    double gWeightedAvg = 0;
    double bWeightedAvg = 0;

    for (int k = -kRadius; k <= kRadius; k++) {
      const double weight = gaussianKernelComponent[k + kRadius];
      const size_t pixel = mirrorPixel(i + k, width);

      rWeightedAvg += weight * this->red(pixel, j);
      gWeightedAvg += weight * this->green(pixel, j);
      bWeightedAvg += weight * this->blue(pixel, j);
    }

    tempPic.set(i, j, rWeightedAvg, gWeightedAvg, bWeightedAvg, 255);
  });

  // vertical second pass writes directly to final picture object
  process2dInParallel(height, width, [&](int i, int j) {
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

    this->set(i, j, rWeightedAvg, gWeightedAvg, bWeightedAvg, 255);
  });
}
