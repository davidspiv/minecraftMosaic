#include <filesystem>
#include <string>
#include <vector>

#include "../include/color.h"
#include "../include/picture.h"
#include "../include/util.h"

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


std::vector<BitMap> getValidTextures(std::vector<std::string> fPaths) {
  const size_t numTiles = fPaths.size();
  std::vector<BitMap> validTextures;

  for (size_t i = 0; i < numTiles; i++) {

    Picture texture(fPaths[i]);

    bool isTransparent = false;

    for (size_t j = 0; j < blockSize && !isTransparent; j++) {
      for (size_t k = 0; k < blockSize; k++) {
        const int a = texture.alpha(k, j);

        if (a != 255) {
          isTransparent = true;
          break;
        }
      }
    }

    if (!isTransparent) {
      BitMap bitMap(16, 16);
      for (size_t j = 0; j < 16; j++) {
        for (size_t k = 0; k < 16; k++) {
          const int r = texture.red(k, j);
          const int g = texture.green(k, j);
          const int b = texture.blue(k, j);

          StdRGB stdRGB(r, g, b);
          bitMap.set(k, j, stdRGB);
        }
      }


      validTextures.push_back(bitMap);
    }
  }

  return validTextures;
}


std::vector<StdRGB>
getTextureAvgColors(const std::vector<BitMap> &validTextures) {
  const size_t numValidTiles = validTextures.size();
  std::vector<StdRGB> avgColors(numValidTiles, StdRGB());

  for (size_t i = 0; i < numValidTiles; i++) {
    BitMap bitMap(validTextures[i]);
    avgColors.at(i) = getAverageRGB(bitMap, 0, 0);
  }

  return avgColors;
}


void createTexturedPic(const BitMap &bitMap,
                       const std::vector<BitMap> &validTextures) {

  const std::vector<StdRGB> textureAvgColors =
      getTextureAvgColors(validTextures);
  const std::vector<std::vector<int>> textureLookupTable =
      buildLookupTable(bitMap, textureAvgColors);

  Picture texturedPic(textureLookupTable.at(0).size() * blockSize,
                      textureLookupTable.size() * blockSize, 0, 0, 0);

  for (int j = 0; j < texturedPic.height(); j++) {
    for (int i = 0; i < texturedPic.width(); i++) {

      const int texIdx = textureLookupTable.at(j / blockSize).at(i / blockSize);
      auto [r, g, b] =
          validTextures.at(texIdx).get(i % blockSize, j % blockSize);

      texturedPic.set(i, j, r, g, b, 255);
    }
  }

  texturedPic.save("./outputPics/texturedPic.png");
}
