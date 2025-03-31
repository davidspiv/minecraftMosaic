#include "../include/avgPic.h"
#include "../include/gaussianBlur.h"
#include "../include/miscPics.h"
#include "../include/picture.h"
#include "../include/texturePic.h"
#include "../include/timer.h"

#include <string>
#include <vector>

int main() {
  Timer timer;
  Picture srcPic("./srcPics/lotus.png");
  //   gaussianBlur(srcPic, 20);

  //   const std::string dir = "./blocks";
  //   const std::vector<std::string> fPaths = getValidPaths(dir);
  //   const std::vector<Picture> validTextures = getValidTextures(fPaths);

  //   createTexturedPic(srcPic, validTextures);
  createQuantizedPic(srcPic);
  //   createAvgPic(srcPic);
  //   createAtlasPic(validTextures);
}
