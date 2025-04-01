#include "../include/avgPic.h"
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
  Picture srcPic("./srcPics/tahoe.png");
  Bitmap bitmap = srcPic.bitmap();

  //   gaussianBlur(srcPic, 20);

  const std::string dir = "./blocks";
  const std::vector<std::string> fPaths = getValidPaths(dir);
  const std::vector<Bitmap> validTextures = getValidTextures(fPaths);

  createTexturedPic(bitmap, validTextures);
  Bitmap quantBitmap = createQuantizedPic(bitmap);
  Picture quantPic(quantBitmap);
  quantPic.save("./outputPics/quantizedPic.png");
  createAvgPic(bitmap);
  createAtlasPic(validTextures);
}
