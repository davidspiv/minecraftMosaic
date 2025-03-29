#include "../include/picture.h"
#include "../include/timer.h"
#include "../include/util.h"

#include <cmath>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>


std::vector<std::string> getValidPaths(std::string dir) {
  std::vector<std::string> fPaths;

  for (const auto &f : std::filesystem::directory_iterator(dir)) {
    if (std::filesystem::is_regular_file(f)) {
      const std::string fPath = f.path();
      if (fPath.find(".png") != std::string::npos) {
        fPaths.push_back(f.path());
      }
    }
  }

  return fPaths;
}


std::vector<Picture> getValidTextures(std::vector<std::string> fPaths) {
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

  return validTextures;
}


std::vector<Color>
getTextureAvgColors(const std::vector<Picture> &validTextures) {
  const size_t numValidTiles = validTextures.size();
  std::vector<Color> avgColors(numValidTiles, Color());

  for (size_t i = 0; i < numValidTiles; i++) {
    Picture pic(validTextures[i]);
    avgColors.at(i) = getAverageRGB(pic, 0, 0);
  }

  return avgColors;
}


int findNearestColor(const Color &targetColor,
                     const std::vector<Color> &quantColors) {
  int nearest = 0;
  double minDist = std::numeric_limits<double>::max();

  for (size_t i = 0; i < quantColors.size(); i++) {
    double dist = distSquared(targetColor, quantColors.at(i));
    if (dist < minDist) {
      minDist = dist;
      nearest = i;
    }
  }

  return nearest;
}


std::vector<std::vector<int>>
buildTextureLookupTable(const Picture &pic,
                        const std::vector<Color> &quantColors) {
  const int cHorizontal = pic.width() / 16;
  const int cVertical = pic.height() / 16;

  std::vector<std::vector<Color>> avgColors(cVertical,
                                            std::vector<Color>(cHorizontal));
  std::vector<std::vector<int>> textureLookupTable(
      cVertical, std::vector<int>(cHorizontal));

  for (int j = 0; j < pic.height(); j += 16) {
    for (int i = 0; i < pic.width(); i += 16) {

      const Color avgColor = getAverageRGB(pic, i, j);
      avgColors.at(j / 16).at(i / 16) = avgColor;

      const int texIdx = findNearestColor(avgColor, quantColors);
      textureLookupTable.at(j / 16).at(i / 16) = texIdx;
    }
  }

  return textureLookupTable;
}


std::vector<std::vector<Color>> buildAvgLookupTable(const Picture &pic) {
  const int cHorizontal = pic.width() / 16;
  const int cVertical = pic.height() / 16;

  std::vector<std::vector<Color>> avgColors(cVertical,
                                            std::vector<Color>(cHorizontal));

  for (int j = 0; j < pic.height(); j += 16) {
    for (int i = 0; i < pic.width(); i += 16) {

      const Color avgColor = getAverageRGB(pic, i, j);
      avgColors.at(j / 16).at(i / 16) = avgColor;
    }
  }

  return avgColors;
}


void createAtlasPic(const std::vector<Picture> &validTextures) {
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

    for (size_t j = 0; j < tileSize; j++) {
      for (size_t k = 0; k < tileSize; k++) {
        const int r = pic.red(k, j);
        const int g = pic.green(k, j);
        const int b = pic.blue(k, j);

        atlas.set(xOffset + k, yOffset + j, r, g, b, 255);
      }
    }
  }

  atlas.save("atlas.png");
}


void createTexturesPic(const Picture &pic,
                       std::vector<std::vector<int>> textureLookupTable,
                       const std::vector<Picture> &validTextures) {

  Picture quantPic(pic.width(), pic.height(), 0, 0, 0);

  for (int j = 0; j < quantPic.height(); j++) {
    for (int i = 0; i < quantPic.width(); i++) {
      const int texIdx = textureLookupTable.at(j / 16).at(i / 16);

      int r = validTextures.at(texIdx).red(i % 16, j % 16);
      int g = validTextures.at(texIdx).green(i % 16, j % 16);
      int b = validTextures.at(texIdx).blue(i % 16, j % 16);

      quantPic.set(i, j, r, g, b, 255);
    }
  }

  quantPic.save("quantPic.png");
}


void createAvgPic(const Picture &pic,
                  const std::vector<std::vector<Color>> &avgColors) {

  Picture avgPic(pic.width(), pic.height(), 0, 0, 0);

  for (int j = 0; j < avgPic.height(); j++) {
    for (int i = 0; i < avgPic.width(); i++) {
      auto [avgR, avgG, avgB] = avgColors.at(j / 16).at(i / 16);
      avgPic.set(i, j, avgR, avgG, avgB, 255);
    }
  }

  avgPic.save("avgPic.png");
}


int main() {
  Timer timer;
  const std::string dir = "./blocks";
  const std::vector<std::string> fPaths = getValidPaths(dir);
  const std::vector<Picture> validTextures = getValidTextures(fPaths);
  const std::vector<Color> avgColors = getTextureAvgColors(validTextures);

  const Picture srcPic("warhammer.png");

  const std::vector<std::vector<int>> textureLookupTable =
      buildTextureLookupTable(srcPic, avgColors);
  const std::vector<std::vector<Color>> avgLookupTable =
      buildAvgLookupTable(srcPic);

  createTexturesPic(srcPic, textureLookupTable, validTextures);
  createAvgPic(srcPic, avgLookupTable);
  createAtlasPic(validTextures);
}
