#include "../include/color.h"
#include "../include/util.h"

#include <vector>

Bitmap getAvgColorPerBlock(const Bitmap &bitmap) {
  const int cHorizontal = (bitmap.width() / blockSize) + 1;
  const int cVertical = (bitmap.height() / blockSize) + 1;

  Bitmap avgColors(cHorizontal, cVertical);

  for (int j = 0; j < bitmap.height(); j += blockSize) {
    for (int i = 0; i < bitmap.width(); i += blockSize) {

      const CieLab avgColor = getAverage(bitmap, i, j);
      avgColors.set(i / blockSize, j / blockSize, avgColor);
    }
  }

  return avgColors;
}


void createAvgPic(const Bitmap &bitmap) {

  const Bitmap avgColors = getAvgColorPerBlock(bitmap);
  Picture avgPic(bitmap.width(), bitmap.height(), 0, 0, 0);

  for (int j = 0; j < avgPic.height(); j++) {
    for (int i = 0; i < avgPic.width(); i++) {
      auto [avgR, avgG, avgB] =
          StdRGB(avgColors.get(i / blockSize, j / blockSize));
      avgPic.set(i, j, avgR, avgG, avgB, 255);
    }
  }

  avgPic.save("./outputPics/avgPic.png");
}
