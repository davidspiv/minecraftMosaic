#include "../include/Bitmap.h"
#include "../include/picture.h"
#include "../include/util.h"

void createAtlasPic(const std::vector<Bitmap> &validTextures) {
  const size_t numValidTiles = validTextures.size();
  const int gridSize =
      std::ceil(std::sqrt(numValidTiles)); // Ensure a square grid

  const int atlasSize = gridSize * BLOCK_SIZE;
  Bitmap bitmapOut(atlasSize, atlasSize);


  for (size_t i = 0; i < numValidTiles; i++) {
    Bitmap bitmap(validTextures[i]);

    int xOffset = (i % gridSize) * BLOCK_SIZE; // Column position
    int yOffset = (i / gridSize) * BLOCK_SIZE; // Row position

    for (size_t j = 0; j < BLOCK_SIZE; j++) {
      for (size_t k = 0; k < BLOCK_SIZE; k++) {

        bitmapOut.m_bits[yOffset + j][xOffset + k] = bitmap.m_bits[j][k];
      }
    }
  }

  Picture atlasPic(bitmapOut, BLOCK_SIZE);
  atlasPic.save("./outputPics/atlasPic.png");
}
