#include "../include/Timer.h"
#include "../include/atlasPic.h"
#include "../include/gaussianBlur.h"
#include "../include/picture.h"
#include "../include/quantizePic.h"
#include "../include/texturePic.h"
#include "../include/util.h"

#include <filesystem>
#include <fstream>
#include <string>
#include <vector>

int main() {
  Timer::global();
  Picture srcPic("./srcPics/van.png");

  srcPic.gaussianBlur(15);
  Picture minPic = srcPic.bilinearResize(ONE_SIXTEENTH);
  Bitmap bitmap = minPic.getBitmap();

  const std::string textureDir = "./blocks";
  std::vector<std::string> fPaths;
  std::vector<Bitmap> validTextures;
  std::vector<clrspc::Lab> textureAvgColors;

  if (!std::filesystem::exists("test.dat")) {
    fPaths = getValidPaths(textureDir);
    validTextures = getValidTextures(fPaths);
    textureAvgColors = calcTextureAvgColors(validTextures);
  } else {
    std::ifstream ifs;
    ifs.open(("test.dat"));
    std::string line;
    while (getline(ifs, line) && !line.empty()) {
      const int comma_idx = line.find(",");
      std::string texturePath = line.substr(0, comma_idx);

      const int l_idx = line.find("l:");
      const int a_idx = line.find("a:");
      const int b_idx = line.find("b:");

      const float l = stof(line.substr(l_idx + 2, line.size() - a_idx));
      const float a = stof(line.substr(a_idx + 2, line.size() - b_idx));
      const float b = stof(line.substr(b_idx + 2));

      clrspc::Lab avgColor(l, a, b);

      fPaths.emplace_back(texturePath);
      textureAvgColors.emplace_back(avgColor);
    }
    validTextures = getValidTextures(fPaths);
    ifs.close();
    if (validTextures.empty() ||
        (validTextures.size() != textureAvgColors.size())) {
      throw std::runtime_error(
          "Unable to import texture data. Delete texture.dat and try again");
    }
  }


  const auto textureLookupTable = buildLookupTable(bitmap, textureAvgColors);

  createTexturedPic(textureLookupTable, validTextures);
  createQuantizedPic(bitmap);
  createAtlasPic(validTextures);

  Timer::printData();
}
