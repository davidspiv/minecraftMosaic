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

std::vector<std::string> getPaths(const std::string &dir) {
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

std::vector<std::string> getValidPaths(const std::vector<std::string> &paths) {
  const size_t numTiles = paths.size();
  std::vector<std::string> validPaths;

  std::ofstream ofs("texture.dat");

  for (size_t i = 0; i < numTiles; i++) {

    Picture texture(paths[i]);

    // TRANSPARENT PIXELS CHECK
    bool isTransparent = false;

    float lAvg = 0;
    float aAvg = 0;
    float bAvg = 0;

    for (size_t j = 0; j < BLOCK_SIZE && !isTransparent; j++) {
      for (size_t k = 0; k < BLOCK_SIZE; k++) {
        const int alpha = texture.alpha(k, j);

        if (alpha != 255) {
          isTransparent = true;
          break;
        }

        auto [l, a, b] = clrspc::Rgb(texture.red(k, j), texture.green(k, j),
                                     texture.blue(k, j))
                             .to_lab()
                             .get_values();

        lAvg += l;
        aAvg += a;
        bAvg += b;
      }
    }

    if (isTransparent)
      continue;

    constexpr float pixelCount = BLOCK_SIZE * BLOCK_SIZE;

    lAvg /= pixelCount;
    aAvg /= pixelCount;
    bAvg /= pixelCount;

    const clrspc::Lab colorAvg(lAvg, aAvg, bAvg);

    // VARIANCE WITHIN TEXTURE CHECK
    const float diffMax = 35;
    float diff = 0;

    for (size_t j = 0; j < BLOCK_SIZE && diff < diffMax; j++) {
      for (size_t k = 0; k < BLOCK_SIZE; k++) {
        const int rCurr = texture.red(k, j);
        const int gCurr = texture.green(k, j);
        const int bCurr = texture.blue(k, j);

        const clrspc::Lab colorCurr = clrspc::Rgb(rCurr, gCurr, bCurr).to_lab();

        diff += distSquared(colorAvg, colorCurr);
      }
    }

    if (diff > diffMax) {
      continue;
    }

    // TEXTURE IS VALID

    // print to cache file
    const auto [l, a, b] = colorAvg.get_values();
    ofs << paths[i] << ", l: " << l << " a: " << a << " b: " << b << '\n';


    validPaths.emplace_back(paths[i]);
  }

  ofs.close();
  return validPaths;
};


std::vector<Bitmap>
getValidTextures(const std::vector<std::string> &validPaths) {
  const size_t numTiles = validPaths.size();
  std::vector<Bitmap> validTextures;

  for (size_t i = 0; i < numTiles; i++) {
    Picture texture(validPaths[i]);

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

  return validTextures;
}


std::vector<clrspc::Lab>
calcTextureAvgColors(const std::vector<Bitmap> &validTextures) {
  const size_t numValidTiles = validTextures.size();
  std::vector<clrspc::Lab> avgColors(numValidTiles, clrspc::Lab(0.f, 0.f, 0.f));

  for (size_t i = 0; i < numValidTiles; i++) {
    Bitmap bitmap(validTextures[i]);
    avgColors.at(i) = getAverage(bitmap, 0, 0);
  }

  return avgColors;
}


void getTextureData(std::vector<Bitmap> &validTextures,
                    std::vector<clrspc::Lab> &textureAvgColors) {

  const std::string textureDir = "./blocks";
  std::vector<std::string> validPaths;


  if (!std::filesystem::exists("texture.dat")) {
    std::vector<std::string> paths = getPaths(textureDir);
    validPaths = getValidPaths(paths);
    validTextures = getValidTextures(validPaths);
    textureAvgColors = calcTextureAvgColors(validTextures);
  } else {
    std::ifstream ifs;
    ifs.open(("texture.dat"));
    std::string line;
    while (getline(ifs, line) && !line.empty()) {

      const size_t commaIdx = line.find(",");

      const size_t lIdx = line.find("l:") + 2;
      const size_t aIdx = line.find("a:") + 2;
      const size_t bIdx = line.find("b:") + 2;

      if (commaIdx == std::string::npos || lIdx == std::string::npos ||
          aIdx == std::string::npos || bIdx == std::string::npos) {
        throw std::runtime_error(
            "Unable to import texture data. Delete texture.dat and try again");
      }

      std::string texturePath = line.substr(0, commaIdx);

      float l = std::stof(line.substr(lIdx, aIdx - lIdx + 2));
      float a = std::stof(line.substr(aIdx, bIdx - aIdx + 2));
      float b = std::stof(line.substr(bIdx));

      clrspc::Lab avgColor(l, a, b);

      validPaths.emplace_back(texturePath);
      textureAvgColors.emplace_back(avgColor);
    }

    validTextures = getValidTextures(validPaths);
    ifs.close();
    if (validTextures.empty() ||
        (validTextures.size() != textureAvgColors.size())) {
      throw std::runtime_error(
          "Unable to import texture data. Delete texture.dat and try again");
    }
  }
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
