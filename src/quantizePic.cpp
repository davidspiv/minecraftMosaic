#include "../include/quantizePic.h"
#include "../include/Bitmap.h"
#include "../include/Color_Space.h"
#include "../include/picture.h"
#include "../include/util.h"

#include <algorithm>
#include <cmath>
#include <thread>
#include <vector>

std::vector<clrspc::Lab> getQuantizedColors()
{
    std::vector<clrspc::Lab> colors;

    for (int i = 0; i <= 255; i += 30) {
        for (int j = 0; j <= 255; j += 30) {
            for (int k = 0; k <= 255; k += 30) {
                clrspc::Rgb const rgb(i, j, k);
                colors.push_back(rgb.to_lab());
            }
        }
    }

    return colors;
}

std::vector<clrspc::Lab> getPalletColors()
{
    //   endesgaPalette, apolloPalette, resurrectPalette, zughyPalette
    std::vector<clrspc::Rgb> const palette = apolloPalette;
    std::vector<clrspc::Lab> colors;
    colors.reserve(palette.size());
    std::transform(palette.begin(), palette.end(), std::back_inserter(colors),
        [](clrspc::Rgb c) { return c.to_lab(); });

    return colors;
}

void createQuantizedPic(Bitmap const& bitmapIn)
{
    //   const std::vector<clrspc::Lab> colors = getPalletColors();
    std::vector<clrspc::Lab> const colors = getQuantizedColors();
    std::vector<std::vector<int>> const lookupTable = buildLookupTable(bitmapIn, colors);

    Bitmap bitmapOut(bitmapIn.m_width, bitmapIn.m_height);

    process2dInParallel(bitmapIn.m_height, bitmapIn.m_width, [&](int i, int j) {
        int const texIdx = lookupTable[j][i];
        bitmapOut.set(i, j, colors[texIdx].to_rgb());
    });

    Picture quantPic(bitmapOut, BLOCK_SIZE);
    quantPic.save("./outputPics/quantizedPic.png");
}
