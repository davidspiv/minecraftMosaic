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

std::vector<CieLab> getPalletColors() {
  //   endesgaPalette, apolloPalette, resurrectPalette, zughyPalette
  const std::vector<StdRGB> palette = apolloPalette;
  std::vector<CieLab> colors(palette.size());
  std::transform(palette.begin(), palette.end(), colors.begin(),
                 [](StdRGB c) { return CieLab(c); });

  return colors;
}


void createQuantizedPic(const Bitmap &bitmapIn) {

  //   const std::vector<CieLab> colors = getQuantizedColors();
  const std::vector<CieLab> colors = getPalletColors();

  const std::vector<std::vector<int>> lookupTable =
      buildLookupTable(bitmapIn, colors);


  Bitmap bitmapOut(bitmapIn.width(), bitmapIn.height());

  for (int j = 0; j < bitmapIn.height(); j++) {
    for (int i = 0; i < bitmapIn.width(); i++) {
      const int texIdx = lookupTable[j][i];

      bitmapOut.set(i, j, colors.at(texIdx));
    }
  }

  Picture quantPic(bitmapOut, blockSize);
  quantPic.save("./outputPics/quantizedPic.png");
}


void createAtlasPic(const std::vector<Bitmap> &validTextures) {
  const size_t numValidTiles = validTextures.size();
  const int gridSize =
      std::ceil(std::sqrt(numValidTiles)); // Ensure a square grid

  const int atlasSize = gridSize * blockSize;
  Bitmap bitmapOut(atlasSize, atlasSize);

  for (size_t i = 0; i < numValidTiles; i++) {
    Bitmap bitmap(validTextures[i]);

    int xOffset = (i % gridSize) * blockSize; // Column position
    int yOffset = (i / gridSize) * blockSize; // Row position

    for (size_t j = 0; j < blockSize; j++) {
      for (size_t k = 0; k < blockSize; k++) {

        bitmapOut.set(xOffset + k, yOffset + j, bitmap.get(k, j));
      }
    }
  }

  Picture atlasPic(bitmapOut, blockSize);
  atlasPic.save("./outputPics/atlasPic.png");
}
