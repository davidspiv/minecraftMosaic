#include "../include/picture.h"

#include <cmath>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

struct Color {
  int r;
  int g;
  int b;
};


Color getAverageRGB(const Picture &pic) {
  // https://sighack.com/post/averaging-rgb-colors-the-right-way

  int r = 0;
  int b = 0;
  int g = 0;

  int numPx = 0;
  for (int i = 0; i < pic.height(); i++) {
    for (int j = 0; j < pic.width(); j++) {

      /* Sum the squares of components instead */
      r += pic.red(j, i) * pic.red(j, i);
      g += pic.green(j, i) * pic.green(j, i);
      b += pic.blue(j, i) * pic.blue(j, i);

      ++numPx;
    }
  }
  /* Return the sqrt of the mean of squared R, G, and B sums */
  return {int(std::round(sqrt(r / numPx))), int(std::round(sqrt(g / numPx))),
          int(std::round(sqrt(b / numPx)))};
}


int main() {
  std::string dir = "./blocks";
  std::vector<std::string> fPaths;

  for (const auto &f : std::filesystem::directory_iterator(dir)) {
    if (std::filesystem::is_regular_file(f)) {
      const std::string fPath = f.path();
      if (fPath.find(".png") != std::string::npos) {
        fPaths.push_back(f.path());
      }
    }
  }

  //   fPaths.push_back("blocks/calibrated_sculk_sensor_amethyst.png");

  const size_t numTiles = fPaths.size();
  std::vector<Picture> validTextures;

  for (size_t i = 0; i < numTiles; i++) {

    Picture texture(fPaths[i]);

    bool isTransparent = false;

    for (size_t j = 0; j < 16 && !isTransparent; j++) {
      for (size_t k = 0; k < 16; k++) {
        const int a = texture.alpha(k, j);

        if (a != 255) {
          isTransparent = true;
          break;
        }
      }
    }

    if (!isTransparent) {
      validTextures.push_back(std::move(texture));
    }
  }

  // STITCH VALID TILES
  const int tileSize = 16;
  const size_t numValidTiles = validTextures.size();
  const int gridSize =
      std::ceil(std::sqrt(numValidTiles)); // Ensure a square grid

  const int atlasSize = gridSize * tileSize;
  Picture atlas(atlasSize, atlasSize, 0, 0, 0);

  for (size_t i = 0; i < numValidTiles; i++) {
    Picture pic(validTextures[i]);

    int xOffset = (i % gridSize) * tileSize; // Column position
    int yOffset = (i / gridSize) * tileSize; // Row position

    Color averageRGB = getAverageRGB(pic);

    for (size_t j = 0; j < tileSize; j++) {
      for (size_t k = 0; k < tileSize; k++) {
        // const int r = pic.red(k, j);
        // const int g = pic.green(k, j);
        // const int b = pic.blue(k, j);

        auto [rA, gA, bA] = averageRGB;

        atlas.set(xOffset + k, yOffset + j, rA, gA, bA, 255);
      }
    }
  }

  atlas.save("atlas.png");
}
