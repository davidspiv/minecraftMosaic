#include "../include/Timer.h"
#include "../include/atlasPic.h"
#include "../include/gaussianBlur.h"
#include "../include/picture.h"
#include "../include/quantizePic.h"
#include "../include/texturePic.h"
#include "../include/util.h"

#include <string>
#include <vector>

int main() {
  Picture srcPic("./srcPics/garden.png");

  gaussianBlur(srcPic, 20);
  Picture minPic = srcPic.bilinearResize(0.0625); // one-sixteenth
  Bitmap bitmap = minPic.getBitmap();

  const std::string textureDir = "./blocks";
  const std::vector<std::string> fPaths = getValidPaths(textureDir);
  const std::vector<Bitmap> validTextures = getValidTextures(fPaths);

  createTexturedPic(bitmap, validTextures);
  //   createQuantizedPic(bitmap);
  //   createAtlasPic(validTextures);

  Timer::printData();
}
