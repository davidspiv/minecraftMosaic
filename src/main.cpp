#include "../include/avgPic.h"
#include "../include/color.h"
#include "../include/gaussianBlur.h"
#include "../include/miscPics.h"
#include "../include/picture.h"
#include "../include/texturePic.h"
#include "../include/timer.h"

#include <string>
#include <vector>

int main() {
  Timer timer;
  Picture srcPic("./srcPics/tahoe.png");
  const size_t width = srcPic.width();
  const size_t height = srcPic.height();

  Bitmap bitmap(width, height);

  for (size_t j = 0; j < height; j++) {
    for (size_t i = 0; i < width; i++) {
      const int r = srcPic.red(i, j);
      const int g = srcPic.green(i, j);
      const int b = srcPic.blue(i, j);

      StdRGB stdRGB(r, g, b);
      CieLab cieLab(stdRGB);

      bitmap.set(i, j, cieLab);
    }
  }
  //   gaussianBlur(srcPic, 20);

  const std::string dir = "./blocks";
  const std::vector<std::string> fPaths = getValidPaths(dir);
  const std::vector<Bitmap> validTextures = getValidTextures(fPaths);

  createTexturedPic(bitmap, validTextures);
  createQuantizedPic(bitmap);
  createAvgPic(bitmap);
  createAtlasPic(validTextures);
}
