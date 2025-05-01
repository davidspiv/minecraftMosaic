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


std::vector<Bitmap> getValidTextures(std::vector<std::string> fPaths) {
  const size_t numTiles = fPaths.size();
  std::vector<Bitmap> validTextures;

  for (size_t i = 0; i < numTiles; i++) {

    Picture texture(fPaths[i]);

    bool isTransparent = false;

    int rAvg = 0;
    int gAvg = 0;
    int bAvg = 0;

    for (size_t j = 0; j < blockSize && !isTransparent; j++) {
      for (size_t k = 0; k < blockSize; k++) {
        const int a = texture.alpha(k, j);

        if (a != 255) {
          isTransparent = true;
          break;
        }

        rAvg += texture.red(k, j);
        gAvg += texture.green(k, j);
        bAvg += texture.blue(k, j);
      }
    }

    if (isTransparent)
      continue;

    const int pixelCount = blockSize * blockSize;

    rAvg /= pixelCount;
    gAvg /= pixelCount;
    bAvg /= pixelCount;

    const StdRGB colorAvg(rAvg, gAvg, bAvg);

    int diff = 0;
    int diffMax = 400'000;

    for (size_t j = 0; j < blockSize && diff < diffMax; j++) {
      for (size_t k = 0; k < blockSize; k++) {
        const int rCurr = texture.red(k, j);
        const int gCurr = texture.green(k, j);
        const int bCurr = texture.blue(k, j);

        const StdRGB colorCurr(rCurr, gCurr, bCurr);

        diff += distSquared(colorAvg, colorCurr);
      }
    }


    if (diff > diffMax)
      continue;

    Bitmap bitmap(blockSize, blockSize);
    for (size_t j = 0; j < blockSize; j++) {
      for (size_t k = 0; k < blockSize; k++) {
        const int r = texture.red(k, j);
        const int g = texture.green(k, j);
        const int b = texture.blue(k, j);

        StdRGB stdRGB(r, g, b);
        bitmap.set(k, j, CieLab(stdRGB));
      }
    }


    validTextures.push_back(bitmap);
  }

  return validTextures;
}


std::vector<CieLab>
getTextureAvgColors(const std::vector<Bitmap> &validTextures) {
  const size_t numValidTiles = validTextures.size();
  std::vector<CieLab> avgColors(numValidTiles, CieLab());

  for (size_t i = 0; i < numValidTiles; i++) {
    Bitmap bitmap(validTextures[i]);
    avgColors.at(i) = getAverage(bitmap, 0, 0);
  }

  return avgColors;
}


void createTexturedPic(const Bitmap &bitmap,
                       const std::vector<Bitmap> &validTextures) {

  const std::vector<CieLab> textureAvgColors =
      getTextureAvgColors(validTextures);
  const std::vector<std::vector<int>> textureLookupTable =
      buildLookupTable(bitmap, textureAvgColors);

  Picture texturedPic(textureLookupTable.at(0).size() * blockSize,
                      textureLookupTable.size() * blockSize, 0, 0, 0);

  for (int j = 0; j < texturedPic.height(); j++) {
    for (int i = 0; i < texturedPic.width(); i++) {
      const int texIdx = textureLookupTable.at(j / blockSize).at(i / blockSize);
      auto [r, g, b] =
          StdRGB(validTextures.at(texIdx).get(i % blockSize, j % blockSize));

      texturedPic.set(i, j, r, g, b, 255);
    }
  }


  texturedPic.save("./outputPics/texturedPic.png");
}
