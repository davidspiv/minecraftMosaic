#include "../include/color.h"
#include "../include/util.h"

#include <vector>

std::vector<std::vector<StdRGB>> getAvgColorPerBlock(const Picture &pic) {
  const int cHorizontal = pic.width() / blockSize;
  const int cVertical = pic.height() / blockSize;

  std::vector<std::vector<StdRGB>> avgColors(cVertical,
                                             std::vector<StdRGB>(cHorizontal));

  for (int j = 0; j < pic.height(); j += blockSize) {
    for (int i = 0; i < pic.width(); i += blockSize) {

      const StdRGB avgColor = getAverageRGB(pic, i, j);
      avgColors.at(j / blockSize).at(i / blockSize) = avgColor;
    }
  }

  return avgColors;
}


void createAvgPic(const Picture &pic) {

  const std::vector<std::vector<StdRGB>> avgColors = getAvgColorPerBlock(pic);
  Picture avgPic(pic.width(), pic.height(), 0, 0, 0);

  for (int j = 0; j < avgPic.height(); j++) {
    for (int i = 0; i < avgPic.width(); i++) {
      auto [avgR, avgG, avgB] = avgColors.at(j / blockSize).at(i / blockSize);
      avgPic.set(i, j, avgR, avgG, avgB, 255);
    }
  }

  avgPic.save("./outputPics/avgPic.png");
}
