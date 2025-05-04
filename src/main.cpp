#include "../include/Timer.h"
#include "../include/atlasPic.h"
#include "../include/config.h"
#include "../include/gaussianBlur.h"
#include "../include/picture.h"
#include "../include/quantizePic.h"
#include "../include/texturePic.h"
#include "../include/util.h"

#include <vector>


int main() {
  Timer::global();
  Picture srcPic("./srcPics/garden.png");

  srcPic.gaussianBlur(GAUSSIAN_BLUR_RADIUS);
  Picture minPic = srcPic.bilinearResize(ONE_SIXTEENTH);
  Bitmap bitmap = minPic.getBitmap();

  std::vector<Bitmap> validTextures;
  std::vector<clrspc::Lab> textureAvgColors;

  getTextureData(validTextures, textureAvgColors);
  const auto textureLookupTable = buildLookupTable(bitmap, textureAvgColors);

  createTexturedPic(textureLookupTable, validTextures);
  createQuantizedPic(bitmap);
  createAtlasPic(validTextures);

  Timer::printData();
}
