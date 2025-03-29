#include "../include/picture.h"
#include "../include/util.h"

#include <cmath>
#include <vector>

void createQuantizedPic(const Picture &srcPic) {

  const std::vector<ColorRGB> discreteColors = getQuantizedColors();
  const std::vector<std::vector<int>> lookupTable =
      buildLookupTable(srcPic, discreteColors);

  Picture quantPic(lookupTable.at(0).size() * blockSize,
                   lookupTable.size() * blockSize, 0, 0, 0);

  for (int j = 0; j < quantPic.height(); j++) {
    for (int i = 0; i < quantPic.width(); i++) {
      const int texIdx = lookupTable.at(j / blockSize).at(i / blockSize);

      auto [r, g, b] = discreteColors.at(texIdx);

      quantPic.set(i, j, r, g, b, 255);
    }
  }

  quantPic.save("quantizedPic.png");
}


void createAtlasPic(const std::vector<Picture> &validTextures) {
  const size_t numValidTiles = validTextures.size();
  const int gridSize =
      std::ceil(std::sqrt(numValidTiles)); // Ensure a square grid

  const int atlasSize = gridSize * blockSize;
  Picture atlas(atlasSize, atlasSize, 0, 0, 0);

  for (size_t i = 0; i < numValidTiles; i++) {
    Picture pic(validTextures[i]);

    int xOffset = (i % gridSize) * blockSize; // Column position
    int yOffset = (i / gridSize) * blockSize; // Row position

    for (size_t j = 0; j < blockSize; j++) {
      for (size_t k = 0; k < blockSize; k++) {
        const int r = pic.red(k, j);
        const int g = pic.green(k, j);
        const int b = pic.blue(k, j);

        atlas.set(xOffset + k, yOffset + j, r, g, b, 255);
      }
    }
  }

  atlas.save("atlas.png");
}
