#include "../include/util.h"
#include "../include/Color_Space.h"
#include "../include/picture.h"
#include "../include/Timer.h"

#include <algorithm>
#include <array>
#include <cmath>


int distSquared(const clrspc::Rgb &colorA, const clrspc::Rgb &colorB) {

  const int xD = colorB.r() - colorA.r();
  const int yD = colorB.g() - colorA.g();
  const int zD = colorB.b() - colorA.b();
  return xD * xD + yD * yD + zD * zD;
}


double distSquared(const clrspc::Lab &colorA, const clrspc::Lab &colorB) {

  const double xD = colorB.l() - colorA.l();
  const double yD = colorB.a() - colorA.a();
  const double zD = colorB.b() - colorA.b();
  return xD * xD + yD * yD + zD * zD;
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


clrspc::Lab getAverage(const Bitmap &bitmap, int originX, int originY) {
  double lStar = 0.0;
  double aStar = 0.0;
  double bStar = 0.0;
  const int maxX = std::min(originX + BLOCK_SIZE, bitmap.width());
  const int maxY = std::min(originY + BLOCK_SIZE, bitmap.height());
  const int numPx = (maxX - originX) * (maxY - originY);

  for (int x = originX; x < maxX; ++x) {
    for (int y = originY; y < maxY; ++y) {
      const clrspc::Lab &cieLabComponent = bitmap.get(x, y);

      const auto [l, a, b] = cieLabComponent.get_values();

      lStar += l;
      aStar += a;
      bStar += b;
    }
  }

  double invNumPx = 1.0 / numPx;
  return clrspc::Lab(lStar * invNumPx, aStar * invNumPx, bStar * invNumPx);
}


size_t findClosestColorIdx(const clrspc::Lab &targetColor,
                           const std::vector<clrspc::Lab> &quantColors) {
  size_t closestColorIdx = 0;
  double minDist = std::numeric_limits<double>::max();

  for (size_t i = 0; i < quantColors.size(); ++i) {
    const double curDist = distSquared(targetColor, quantColors[i]);

    if (curDist < minDist) {
      minDist = curDist;
      closestColorIdx = i;
    }
  }

  return closestColorIdx;
}


std::vector<std::vector<int>>
buildLookupTable(const Bitmap &bitmap,
                 const std::vector<clrspc::Lab> &quantColors) {
  std::vector<std::vector<int>> lookupTable(bitmap.height(),
                                            std::vector<int>(bitmap.width()));

  for (int i = 0; i < bitmap.width(); i++) {
    for (int j = 0; j < bitmap.height(); j++) {


      const int texIdx = findClosestColorIdx(bitmap.get(i, j), quantColors);

      lookupTable[j][i] = texIdx;
    }
  }

  return lookupTable;
}


void saveAsPNG(const Bitmap &bitmap) {
  Picture quantPic(bitmap.width(), bitmap.height(), 0, 0, 0);

  for (int i = 0; i < bitmap.width(); i++) {
    for (int j = 0; j < bitmap.height(); j++) {
      auto [r, g, b] = bitmap.get(i, j).to_xyz().to_rgb().get_values();

      quantPic.set(i, j, r, g, b);
    }
  }

  quantPic.save("./outputPics/quantizedPic.png");
}
