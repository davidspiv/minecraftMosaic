#include "../include/color.h"
#include "../include/gaussianBlur.h"
#include "../include/miscPics.h"
#include "../include/picture.h"
#include "../include/texturePic.h"
#include "../include/timer.h"
#include "../include/util.h"

#include <string>
#include <vector>

int main() {

  Timer timer;
  Picture srcPic("./srcPics/road.png");

  Picture minPic = srcPic.bilinearResize(0.0625);
  Bitmap bitmap = minPic.bitmap();

  //   gaussianBlur(srcPic, 20);

  const std::string dir = "./blocks";
  const std::vector<std::string> fPaths = getValidPaths(dir);
  const std::vector<Bitmap> validTextures = getValidTextures(fPaths);

  createTexturedPic(bitmap, validTextures);
  Bitmap quantBitmap = createQuantizedPic(bitmap);
  Bitmap atlasBitmap = createAtlasPic(validTextures);

  //   Picture texturePic(textureBitmap);
  Picture quantPic(quantBitmap);
  Picture atlasPic(atlasBitmap);

  //   texturePic.save("./outputPics/texturedPic.png");
  quantPic.save("./outputPics/quantizedPic.png");
  atlasPic.save("./outputPics/atlasPic.png");
}
