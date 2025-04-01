#include "../include/miscPics.h"
#include "../include/color.h"
#include "../include/picture.h"
#include "../include/util.h"

#include <algorithm>
#include <cmath>
#include <vector>


std::vector<CieLab> getQuantizedColors() {
  std::vector<CieLab> colors;

  for (int i = 0; i <= 255; i += 15) {
    for (int j = 0; j <= 255; j += 15) {
      for (int k = 0; k <= 255; k += 15) {
        const StdRGB stdRGB(i, j, k);
        colors.push_back(CieLab(stdRGB));
      }
    }
  }

  return colors;
}


Bitmap createQuantizedPic(const Bitmap &bitmapIn) {

  //   std::vector<CieLab> discreteColors(apolloPalette.size());
  //   std::transform(apolloPalette.begin(), apolloPalette.end(),
  //                  discreteColors.begin(), [](StdRGB c) { return CieLab(c);
  //                  });

  const std::vector<CieLab> discreteColors = getQuantizedColors();

  const std::vector<std::vector<int>> lookupTable =
      buildLookupTable(bitmapIn, discreteColors);


  Bitmap bitmapOut(bitmapIn.width(), bitmapIn.height());

  for (int j = 0; j < bitmapIn.height(); j++) {
    for (int i = 0; i < bitmapIn.width(); i++) {
      const int texIdx = lookupTable.at(j / blockSize).at(i / blockSize);

      bitmapOut.set(i, j, discreteColors.at(texIdx));
    }
  }

  return bitmapOut;
}


void createAtlasPic(const std::vector<Bitmap> &validTextures) {
  const size_t numValidTiles = validTextures.size();
  const int gridSize =
      std::ceil(std::sqrt(numValidTiles)); // Ensure a square grid

  const int atlasSize = gridSize * blockSize;
  Picture atlas(atlasSize, atlasSize, 0, 0, 0);

  for (size_t i = 0; i < numValidTiles; i++) {
    Bitmap bitmap(validTextures[i]);

    int xOffset = (i % gridSize) * blockSize; // Column position
    int yOffset = (i / gridSize) * blockSize; // Row position

    for (size_t j = 0; j < blockSize; j++) {
      for (size_t k = 0; k < blockSize; k++) {
        auto [r, g, b] = StdRGB(bitmap.get(k, j));

        atlas.set(xOffset + k, yOffset + j, r, g, b, 255);
      }
    }
  }

  atlas.save("./outputPics/atlas.png");
}
