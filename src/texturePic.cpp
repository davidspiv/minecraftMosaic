#include "../include/Color_Space.h"
#include "../include/Timer.h"
#include "../include/config.h"
#include "../include/picture.h"
#include "../include/util.h"

#include <cstring>
#include <filesystem>
#include <fstream>
#include <string>
#include <thread>
#include <vector>

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


std::vector<std::pair<std::string, clrspc::Lab>>
getValidTexturesWithAvgs(const std::vector<std::string> &paths) {
  std::vector<std::pair<std::string, clrspc::Lab>> results;

  for (const auto &path : paths) {
    Picture texture(path);
    bool isTransparent = false;

    float lAvg = 0, aAvg = 0, bAvg = 0;

    for (size_t j = 0; j < BLOCK_SIZE && !isTransparent; ++j) {
      for (size_t k = 0; k < BLOCK_SIZE; ++k) {
        if (texture.alpha(k, j) != 255) {
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

    clrspc::Lab avg(lAvg, aAvg, bAvg);

    float diff = 0;
    for (size_t j = 0; j < BLOCK_SIZE && diff < 35; ++j) {
      for (size_t k = 0; k < BLOCK_SIZE; ++k) {
        auto color = clrspc::Rgb(texture.red(k, j), texture.green(k, j),
                                 texture.blue(k, j))
                         .to_lab();
        diff += distSquared(avg, color);
      }
    }

    if (diff > DIFF_THRESHOLD)
      continue;

    results.emplace_back(path, avg);
  }

  return results;
}


void writeTextureCache(
    const std::vector<std::pair<std::string, clrspc::Lab>> &data) {
  std::ofstream ofs("texture.dat", std::ios::binary);
  for (const auto &[path, color] : data) {
    const auto [l, a, b] = color.get_values();
    const uint32_t len = path.size();

    ofs.write(reinterpret_cast<const char *>(&len), sizeof(len));
    ofs.write(path.data(), len);
    ofs.write(reinterpret_cast<const char *>(&l), sizeof(float));
    ofs.write(reinterpret_cast<const char *>(&a), sizeof(float));
    ofs.write(reinterpret_cast<const char *>(&b), sizeof(float));
  }
}


bool readTextureCache(std::vector<std::string> &paths,
                      std::vector<clrspc::Lab> &colors) {
  std::ifstream ifs("texture.dat", std::ios::binary);
  if (!ifs)
    return false;

  while (ifs.peek() != EOF) {
    uint32_t len;
    if (!ifs.read(reinterpret_cast<char *>(&len), sizeof(len)))
      break;

    std::string path(len, '\0');
    if (!ifs.read(&path[0], len))
      break;

    float l, a, b;
    if (!ifs.read(reinterpret_cast<char *>(&l), sizeof(float)) ||
        !ifs.read(reinterpret_cast<char *>(&a), sizeof(float)) ||
        !ifs.read(reinterpret_cast<char *>(&b), sizeof(float)))
      break;

    paths.emplace_back(std::move(path));
    colors.emplace_back(l, a, b);
  }

  return !paths.empty() && paths.size() == colors.size();
}


std::vector<Bitmap> buildBitmaps(const std::vector<std::string> &validPaths) {
  const size_t numTiles = validPaths.size();
  std::vector<Bitmap> validTextures;

  for (size_t i = 0; i < numTiles; i++) {
    Bitmap bitmap = Picture(validPaths[i]).getBitmap();
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
    avgColors.emplace_back(getAverage(bitmap, 0, 0));
  }

  return avgColors;
}


void getTextureData(std::vector<Bitmap> &validTextures,
                    std::vector<clrspc::Lab> &textureAvgColors) {
  Timer timer("getTextureData");
  std::vector<std::string> validPaths;

  if (!readTextureCache(validPaths, textureAvgColors)) {
    auto allPaths = getPaths("./blocks");
    auto textureData = getValidTexturesWithAvgs(allPaths);

    validPaths.reserve(textureData.size());
    textureAvgColors.reserve(textureData.size());

    for (const auto &[path, lab] : textureData) {
      validPaths.push_back(path);
      textureAvgColors.push_back(lab);
    }

    writeTextureCache(textureData);
    validTextures = buildBitmaps(validPaths);
  } else {
    validTextures = buildBitmaps(validPaths);
    if (validTextures.size() != textureAvgColors.size()) {
      throw std::runtime_error("Cached texture data is corrupt.");
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
    Timer timer("createTexturedPic");
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
