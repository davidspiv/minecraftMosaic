#include "../include/gaussianBlur.h"
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


std::vector<ColorRGB>
getTextureAvgColors(const std::vector<Picture> &validTextures) {
  const size_t numValidTiles = validTextures.size();
  std::vector<ColorRGB> avgColors(numValidTiles, ColorRGB());

  for (size_t i = 0; i < numValidTiles; i++) {
    Picture pic(validTextures[i]);
    avgColors.at(i) = getAverageRGB(pic, 0, 0);
  }

  return avgColors;
}


size_t findClosestColorIdx(const ColorRGB &targetColor,
                           const std::vector<ColorRGB> &quantColors) {
  size_t closestColorIdx = 0;
  double minDist = std::numeric_limits<double>::max();

  const ColorXYZ tColorCEI = rgbToCIE(linearize(targetColor));
  const Coord targetCoord = {tColorCEI.x, tColorCEI.y, tColorCEI.z};

  for (size_t i = 0; i < quantColors.size(); i++) {

    const ColorXYZ oColorCEI = rgbToCIE(linearize(quantColors.at(i)));
    const Coord otherCoord = {oColorCEI.x, oColorCEI.y, oColorCEI.z};

    double dist = distSquared(targetCoord, otherCoord);
    if (dist < minDist) {
      minDist = dist;
      closestColorIdx = i;
    }
  }

  return closestColorIdx;
}


std::vector<std::vector<int>>
buildTextureLookupTable(const Picture &pic,
                        const std::vector<ColorRGB> &quantColors) {
  const int blockSize = 16;
  const int cHorizontal = (pic.width() + blockSize - 1) / blockSize;
  const int cVertical = (pic.height() + blockSize - 1) / blockSize;

  std::vector<std::vector<ColorRGB>> avgColors(
      cVertical, std::vector<ColorRGB>(cHorizontal));
  std::vector<std::vector<int>> textureLookupTable(
      cVertical, std::vector<int>(cHorizontal));

  for (int j = 0; j < pic.height(); j += blockSize) {
    for (int i = 0; i < pic.width(); i += blockSize) {

      const ColorRGB avgColor = getAverageRGB(pic, i, j);
      const int newJ = j / blockSize;
      const int newI = i / blockSize;

      avgColors.at(newJ).at(newI) = avgColor;

      const int texIdx = findClosestColorIdx(avgColor, quantColors);
      textureLookupTable.at(newJ).at(newI) = texIdx;
    }
  }

  return textureLookupTable;
}


std::vector<std::vector<ColorRGB>> buildAvgLookupTable(const Picture &pic) {
  const int cHorizontal = pic.width() / 16;
  const int cVertical = pic.height() / 16;

  std::vector<std::vector<ColorRGB>> avgColors(
      cVertical, std::vector<ColorRGB>(cHorizontal));

  for (int j = 0; j < pic.height(); j += 16) {
    for (int i = 0; i < pic.width(); i += 16) {

      const ColorRGB avgColor = getAverageRGB(pic, i, j);
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


void createTexturedPic(const Picture &pic,
                       std::vector<std::vector<int>> textureLookupTable,
                       const std::vector<Picture> &validTextures) {

  Picture texturedPic(pic.width(), pic.height(), 0, 0, 0);

  for (int j = 0; j < texturedPic.height(); j++) {
    for (int i = 0; i < texturedPic.width(); i++) {
      const int texIdx = textureLookupTable.at(j / 16).at(i / 16);

      int r = validTextures.at(texIdx).red(i % 16, j % 16);
      int g = validTextures.at(texIdx).green(i % 16, j % 16);
      int b = validTextures.at(texIdx).blue(i % 16, j % 16);

      texturedPic.set(i, j, r, g, b, 255);
    }
  }

  texturedPic.save("texturedPic.png");
}


void createAvgPic(const Picture &pic,
                  const std::vector<std::vector<ColorRGB>> &avgColors) {

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
  const std::vector<ColorRGB> avgColors = getTextureAvgColors(validTextures);

  Picture srcPic("rainbow.png");
  //   gaussianBlur(srcPic, 15);

  const std::vector<std::vector<int>> textureLookupTable =
      buildTextureLookupTable(srcPic, avgColors);
  //   const std::vector<std::vector<ColorRGB>> avgLookupTable =
  //       buildAvgLookupTable(srcPic);

  createTexturedPic(srcPic, textureLookupTable, validTextures);
  //   createAvgPic(srcPic, avgLookupTable);
  //   createAtlasPic(validTextures);
}
