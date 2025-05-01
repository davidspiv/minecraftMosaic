#include <filesystem>
#include <string>
#include <vector>

#include "../include/Color_Space.h"
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

    const clrspc::Rgb colorAvg(rAvg, gAvg, bAvg);

    int diff = 0;
    int diffMax = 400'000;

    for (size_t j = 0; j < blockSize && diff < diffMax; j++) {
      for (size_t k = 0; k < blockSize; k++) {
        const int rCurr = texture.red(k, j);
        const int gCurr = texture.green(k, j);
        const int bCurr = texture.blue(k, j);

        const clrspc::Rgb colorCurr(rCurr, gCurr, bCurr);

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

        clrspc::Rgb rgb(r, g, b);
        bitmap.set(k, j, rgb.to_xyz().to_lab());
      }
    }


    validTextures.push_back(bitmap);
  }

  return validTextures;
}


std::vector<clrspc::Lab>
getTextureAvgColors(const std::vector<Bitmap> &validTextures) {
  const size_t numValidTiles = validTextures.size();
  std::vector<clrspc::Lab> avgColors(numValidTiles, clrspc::Lab(0.f, 0.f, 0.f));

  for (size_t i = 0; i < numValidTiles; i++) {
    Bitmap bitmap(validTextures[i]);
    avgColors.at(i) = getAverage(bitmap, 0, 0);
  }

  return avgColors;
}


void createTexturedPic(const Bitmap &bitmapIn,
                       const std::vector<Bitmap> &validTextures) {
  const auto textureAvgColors = getTextureAvgColors(validTextures);
  const auto textureLookupTable = buildLookupTable(bitmapIn, textureAvgColors);

  const int outWidth = textureLookupTable[0].size() * blockSize;
  const int outHeight = textureLookupTable.size() * blockSize;
  Picture texturedPic(outWidth, outHeight, 0, 0, 0);

  for (size_t blockY = 0; blockY < textureLookupTable.size(); ++blockY) {
    for (size_t blockX = 0; blockX < textureLookupTable[0].size(); ++blockX) {
      const int texIdx = textureLookupTable[blockY][blockX];
      const Bitmap &texture = validTextures[texIdx];

      for (int y = 0; y < blockSize; ++y) {
        for (int x = 0; x < blockSize; ++x) {
          int outX = blockX * blockSize + x;
          int outY = blockY * blockSize + y;

          auto [r, g, b] = texture.get(x, y).to_xyz().to_rgb().get_values();
          texturedPic.set(outX, outY, r, g, b);
        }
      }
    }
  }

  texturedPic.save("./outputPics/texturedPic.png");
}
