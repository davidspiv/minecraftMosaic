#include <filesystem>
#include <string>
#include <vector>

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


std::vector<Picture> getValidTextures(std::vector<std::string> fPaths) {
  const size_t numTiles = fPaths.size();
  std::vector<Picture> validTextures;

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


void createTexturedPic(const Picture &srcPic,
                       const std::vector<Picture> &validTextures) {

  const std::vector<ColorRGB> textureAvgColors =
      getTextureAvgColors(validTextures);
  const std::vector<std::vector<int>> textureLookupTable =
      buildLookupTable(srcPic, textureAvgColors);

  Picture texturedPic(textureLookupTable.at(0).size() * blockSize,
                      textureLookupTable.size() * blockSize, 0, 0, 0);

  for (int j = 0; j < texturedPic.height(); j++) {
    for (int i = 0; i < texturedPic.width(); i++) {
      const int texIdx = textureLookupTable.at(j / blockSize).at(i / blockSize);

      int r = validTextures.at(texIdx).red(i % blockSize, j % blockSize);
      int g = validTextures.at(texIdx).green(i % blockSize, j % blockSize);
      int b = validTextures.at(texIdx).blue(i % blockSize, j % blockSize);

      texturedPic.set(i, j, r, g, b, 255);
    }
  }

  texturedPic.save("./outputPics/texturedPic.png");
}
