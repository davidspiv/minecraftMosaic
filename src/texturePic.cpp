#include <cstring>
#include <filesystem>
#include <fstream>
#include <string>
#include <thread>
#include <vector>

#include "../include/Color_Space.h"
#include "../include/Timer.h"
#include "../include/picture.h"
#include "../include/util.h"

std::vector<std::string> getValidPaths(const std::string &dir) {
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


std::vector<Bitmap> getValidTextures(const std::vector<std::string> &fPaths) {
  const size_t numTiles = fPaths.size();
  std::vector<Bitmap> validTextures;

  std::ofstream ofs("test.dat");

  for (size_t i = 0; i < numTiles; i++) {

    Picture texture(fPaths[i]);

    // TRANSPARENT PIXELS CHECK
    bool isTransparent = false;

    int rAvg = 0;
    int gAvg = 0;
    int bAvg = 0;

    for (size_t j = 0; j < BLOCK_SIZE && !isTransparent; j++) {
      for (size_t k = 0; k < BLOCK_SIZE; k++) {
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

    constexpr float pixelCount = BLOCK_SIZE * BLOCK_SIZE;

    rAvg /= pixelCount;
    gAvg /= pixelCount;
    bAvg /= pixelCount;

    const clrspc::Rgb colorAvg(rAvg, gAvg, bAvg);

    // VARIANCE WITHIN TEXTURE CHECK
    const int diffMax = 400'000;
    int diff = 0;

    for (size_t j = 0; j < BLOCK_SIZE && diff < diffMax; j++) {
      for (size_t k = 0; k < BLOCK_SIZE; k++) {
        const int rCurr = texture.red(k, j);
        const int gCurr = texture.green(k, j);
        const int bCurr = texture.blue(k, j);

        const clrspc::Rgb colorCurr(rCurr, gCurr, bCurr);

        diff += distSquared(colorAvg, colorCurr);
      }
    }

    if (diff > diffMax)
      continue;

    // TEXTURE IS VALID

    const auto [l, a, b] = colorAvg.to_lab().get_values();
    ofs << fPaths[i] << ", l: " << l << " a: " << a << " b: " << b << '\n';

    Bitmap bitmap(BLOCK_SIZE, BLOCK_SIZE);
    for (size_t j = 0; j < BLOCK_SIZE; j++) {
      for (size_t k = 0; k < BLOCK_SIZE; k++) {
        const int r = texture.red(k, j);
        const int g = texture.green(k, j);
        const int b = texture.blue(k, j);

        bitmap.set(k, j, clrspc::Rgb(r, g, b));
      }
    }


    validTextures.push_back(bitmap);
  }

  ofs.close();
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


void createTexturedPic(const std::vector<std::vector<int>> &textureLookupTable,
                       const std::vector<Bitmap> &validTextures) {

  const int blocksY = textureLookupTable.size();
  const int blocksX = textureLookupTable[0].size();
  const int outWidth = blocksX * BLOCK_SIZE;
  const int outHeight = blocksY * BLOCK_SIZE;

  Picture texturedPic(outWidth, outHeight, 0, 0, 0);

  process2dInParallel(blocksY, blocksX, [&](int blockX, int blockY) {
    const int texIdx = textureLookupTable[blockY][blockX];
    const Bitmap &texture = validTextures[texIdx];

    const int baseX = blockX * BLOCK_SIZE;
    const int baseY = blockY * BLOCK_SIZE;

    for (int y = 0; y < BLOCK_SIZE; ++y) {
      const int outY = baseY + y;

      // Grab 16 RGB pixels (one row), convert to 8 RGBA in one shot
      std::array<uchar, 64> rgbaRow = texture.getSixteen(0, y);

      // Destination index in final RGBA buffer
      const int dst = 4 * (outY * outWidth + baseX);

      // Copy 16 RGBA pixels (64 bytes)
      std::memcpy(&texturedPic._values[dst], rgbaRow.data(), 64);
    }
  });

  texturedPic.save("./outputPics/texturedPic.png");
}
