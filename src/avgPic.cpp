#include "../include/color.h"
#include "../include/util.h"

#include <vector>

BitMap getAvgColorPerBlock(const BitMap &bitMap) {
  const int cHorizontal = bitMap.width() / blockSize;
  const int cVertical = bitMap.height() / blockSize;

  BitMap avgColors(cHorizontal, cVertical);

  for (int j = 0; j < bitMap.height(); j += blockSize) {
    for (int i = 0; i < bitMap.width(); i += blockSize) {

      const StdRGB avgColor = getAverageRGB(bitMap, i, j);
      avgColors.get(i / blockSize, j / blockSize) = avgColor;
    }
  }

  return avgColors;
}


void createAvgPic(const BitMap &bitMap) {

  const BitMap avgColors = getAvgColorPerBlock(bitMap);
  Picture avgPic(bitMap.width(), bitMap.height(), 0, 0, 0);

  for (int j = 0; j < avgPic.height(); j++) {
    for (int i = 0; i < avgPic.width(); i++) {
      auto [avgR, avgG, avgB] = avgColors.get(i / blockSize, j / blockSize);
      avgPic.set(i, j, avgR, avgG, avgB, 255);
    }
  }

  avgPic.save("./outputPics/avgPic.png");
}
