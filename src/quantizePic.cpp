#include "../include/quantizePic.h"
#include "../include/Bitmap.h"
#include "../include/Color_Space.h"
#include "../include/picture.h"
#include "../include/util.h"

#include <algorithm>
#include <cmath>
#include <thread>
#include <vector>


std::vector<clrspc::Lab> getQuantizedColors() {
  std::vector<clrspc::Lab> colors;

  for (int i = 0; i <= 255; i += 60) {
    for (int j = 0; j <= 255; j += 60) {
      for (int k = 0; k <= 255; k += 60) {
        const clrspc::Rgb rgb(i, j, k);
        colors.push_back(rgb.to_lab());
      }
    }
  }

  return colors;
}

std::vector<clrspc::Lab> getPalletColors() {
  //   endesgaPalette, apolloPalette, resurrectPalette, zughyPalette
  const std::vector<clrspc::Rgb> palette = apolloPalette;
  std::vector<clrspc::Lab> colors;
  colors.reserve(palette.size());
  std::transform(palette.begin(), palette.end(), std::back_inserter(colors),
                 [](clrspc::Rgb c) { return c.to_lab(); });

  return colors;
}


void createQuantizedPic(const Bitmap &bitmapIn) {
  //   const std::vector<clrspc::Lab> colors = getPalletColors();
  const std::vector<clrspc::Lab> colors = getQuantizedColors();
  const std::vector<std::vector<int>> lookupTable =
      buildLookupTable(bitmapIn, colors);

  Bitmap bitmapOut(bitmapIn.width(), bitmapIn.height());

  process2dInParallel(bitmapIn.height(), bitmapIn.width(), [&](int i, int j) {
    const int texIdx = lookupTable[j][i];
    bitmapOut.set(i, j, colors[texIdx]);
  });

  Picture quantPic(bitmapOut, BLOCK_SIZE);
  quantPic.save("./outputPics/quantizedPic.png");
}
