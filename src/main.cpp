#include "../include/Timer.h"
#include "../include/gaussianBlur.h"
#include "../include/miscPics.h"
#include "../include/picture.h"
#include "../include/texturePic.h"
#include "../include/util.h"

#include <string>
#include <vector>

int main() {

  Timer timer("Program total");
  Picture srcPic("./srcPics/road.png");

  gaussianBlur(srcPic, 20);
  Picture minPic = srcPic.bilinearResize(0.0625); // one-sixteenth
  Bitmap bitmap = minPic.getBitmap();


  const std::string textureDir = "./blocks";
  const std::vector<std::string> fPaths = getValidPaths(textureDir);
  const std::vector<Bitmap> validTextures = getValidTextures(fPaths);

  createTexturedPic(bitmap, validTextures);
  //   createQuantizedPic(bitmap);
  //   createAtlasPic(validTextures);
}
