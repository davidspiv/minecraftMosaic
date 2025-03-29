#include "../include/picture.h"
#include "../include/timer.h"

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

// convert sRGB channel to a linear value
double linearizeRGB(int channel) {
  double normalizedRGB = channel / 255.0;

  if (normalizedRGB <= 0.04045) {
    return normalizedRGB / 12.92;
  } else {
    return pow(((normalizedRGB + 0.055) / 1.055), 2.4);
  }
}

double distSquared(const Color &colorA, const Color &colorB) {
  const double rD = colorB.r - colorA.r;
  const double gD = colorB.g - colorA.g;
  const double bD = colorB.b - colorA.b;
  return rD * rD + gD * gD + bD * bD;
}

Color getAverageRGB(const Picture &pic) {
  // https://sighack.com/post/averaging-rgb-colors-the-right-way

  int r = 0;
  int b = 0;
  int g = 0;

  int numPx = 0;
  for (int j = 0; j < pic.height(); j++) {
    for (int i = 0; i < pic.width(); i++) {

      /* Sum the squares of components instead */
      r += pic.red(i, j) * pic.red(i, j);
      g += pic.green(i, j) * pic.green(i, j);
      b += pic.blue(i, j) * pic.blue(i, j);

      ++numPx;
    }
  }
  /* Return the sqrt of the mean of squared R, G, and B sums */
  return {int(std::round(sqrt(r / numPx))), int(std::round(sqrt(g / numPx))),
          int(std::round(sqrt(b / numPx)))};
}

Color getAverageRGB(const Picture &pic, int originX, int originY) {

  int r = 0;
  int b = 0;
  int g = 0;

  int numPx = 0;
  for (int j = originY; j < originY + 16; j++) {
    for (int i = originX; i < originX + 16; i++) {

      /* Sum the squares of components instead */
      r += pic.red(i, j) * pic.red(i, j);
      g += pic.green(i, j) * pic.green(i, j);
      b += pic.blue(i, j) * pic.blue(i, j);

      ++numPx;
    }
  }
  /* Return the sqrt of the mean of squared R, G, and B sums */
  return {int(std::round(sqrt(r / numPx))), int(std::round(sqrt(g / numPx))),
          int(std::round(sqrt(b / numPx)))};
}

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
buildQuantizedColors(const std::vector<Picture> &validTextures) {
  const size_t numValidTiles = validTextures.size();
  std::vector<Color> quantColors(numValidTiles, Color());

  for (size_t i = 0; i < numValidTiles; i++) {
    Picture pic(validTextures[i]);
    Color averageRGB = getAverageRGB(pic);
    quantColors.at(i) = averageRGB;
  }

  return quantColors;
}

int findNearestColor(const Color &target,
                     const std::vector<Color> &quantColors) {
  int nearest = 0;
  double minDist = std::numeric_limits<double>::max();

  for (size_t i = 0; i < quantColors.size(); i++) {
    double dist = distSquared(target, quantColors.at(i));
    if (dist < minDist) {
      minDist = dist;
      nearest = i;
    }
  }

  return nearest;
}

void buildAtlas(const std::vector<Picture> &validTextures) {
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

std::vector<std::vector<int>>
buildTextureMap(const Picture &pic, const std::vector<Color> &quantColors) {
  const int cHorizontal = pic.width() / 16;
  const int cVertical = pic.height() / 16;

  std::vector<std::vector<Color>> avgColors(cVertical,
                                            std::vector<Color>(cHorizontal));
  std::vector<std::vector<int>> textureMap(cVertical,
                                           std::vector<int>(cHorizontal));

  for (int j = 0; j < pic.height(); j += 16) {
    for (int i = 0; i < pic.width(); i += 16) {

      const Color avgColor = getAverageRGB(pic, i, j);
      avgColors.at(j / 16).at(i / 16) = avgColor;

      const int texIdx = findNearestColor(avgColor, quantColors);
      textureMap.at(j / 16).at(i / 16) = texIdx;
    }
  }

  return textureMap;
}

std::vector<std::vector<Color>> buildAvgMap(const Picture &pic) {
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

void createTexturesPic(const Picture &pic,
                       std::vector<std::vector<int>> textureMap,
                       const std::vector<Picture> &validTextures) {

  Picture quantPic(pic.width(), pic.height(), 0, 0, 0);

  for (int j = 0; j < quantPic.height(); j++) {
    for (int i = 0; i < quantPic.width(); i++) {
      const int texIdx = textureMap.at(j / 16).at(i / 16);

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

void createTestPic(const Color &exactColor,
                   const std::vector<Picture> &validTextures,
                   const std::vector<Color> &quantColors) {

  auto [exactR, exactG, exactB] = exactColor;
  Picture testPic(32, 16, exactR, exactG, exactB);

  const int texIdx = findNearestColor({exactR, exactG, exactB}, quantColors);


  for (int j = 0; j < testPic.height(); j++) {
    for (int i = testPic.width() / 2; i < testPic.width(); i++) {


      int r = validTextures.at(texIdx).red(i - (testPic.width() / 2), j);
      int g = validTextures.at(texIdx).green(i - (testPic.width() / 2), j);
      int b = validTextures.at(texIdx).blue(i - (testPic.width() / 2), j);

      testPic.set(i, j, r, g, b, 255);
    }
  }

  testPic.save("testPic.png");
}

int main() {
  Timer timer;
  //   const std::string dir = "./blocks";
  //   const std::vector<std::string> fPaths = getValidPaths(dir);
  //   const std::vector<Picture> validTextures = getValidTextures(fPaths);
  //   const std::vector<Color> quantColors =
  //   buildQuantizedColors(validTextures);

  Picture pic("warhammer.png");

  //   const auto textureMap = buildTextureMap(pic, quantColors);
  //   createTexturesPic(pic, textureMap, validTextures);

  const std::vector<std::vector<Color>> avgMap = buildAvgMap(pic);
  createAvgPic(pic, avgMap);

  //   const Color testColor = {167, 118, 96};
  //   createTestPic(testColor, validTextures, quantColors);
  //   buildAtlas(validTextures);
}
