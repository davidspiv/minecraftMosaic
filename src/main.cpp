#include "../include/avgPic.h"
#include "../include/gaussianBlur.h"
#include "../include/miscPics.h"
#include "../include/picture.h"
#include "../include/texturePic.h"
#include "../include/timer.h"
#include "../include/util.h"

#include <cmath>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

int main() {
  Timer timer;
  const Picture srcPic("warhammer.png");

  const std::string dir = "./blocks";
  const std::vector<std::string> fPaths = getValidPaths(dir);
  const std::vector<Picture> validTextures = getValidTextures(fPaths);

  createTexturedPic(srcPic, validTextures);
  createQuantizedPic(srcPic);
  createAvgPic(srcPic);
  createAtlasPic(validTextures);
}
