#include "../include/util.h"
#include "../include/Color_Space.h"
#include "../include/Timer.h"
#include "../include/picture.h"

#include <algorithm>
#include <array>
#include <cmath>

float distSquared(clrspc::Lab const& colorA, clrspc::Lab const& colorB)
{
    float const xD = colorB.l() - colorA.l();
    float const yD = colorB.a() - colorA.a();
    float const zD = colorB.b() - colorA.b();
    return xD * xD + yD * yD + zD * zD;
}

std::array<float, 3> multiplyMatrix(
    std::array<std::array<float, 3>, 3> const& matrix, std::array<float, 3> const& vector)
{

    std::array<float, 3> result = { 0.0, 0.0, 0.0 };

    for (size_t i = 0; i < 3; i++) {
        for (size_t j = 0; j < 3; j++) {
            result[i] += matrix[i][j] * vector[j];
        }
    }

    return result;
}

clrspc::Lab getAverage(Bitmap const& bitmap, int originX, int originY)
{
    float lStar = 0.0;
    float aStar = 0.0;
    float bStar = 0.0;
    int const maxX = std::min(originX + BLOCK_SIZE, bitmap.m_width);
    int const maxY = std::min(originY + BLOCK_SIZE, bitmap.m_height);
    int const numPx = (maxX - originX) * (maxY - originY);

    for (int x = originX; x < maxX; ++x) {
        for (int y = originY; y < maxY; ++y) {
            clrspc::Lab const& labComponent = bitmap.get(x, y).to_lab();

            auto const [l, a, b] = labComponent.get_values();

            lStar += l;
            aStar += a;
            bStar += b;
        }
    }

    float invNumPx = 1.0 / numPx;
    return clrspc::Lab(lStar * invNumPx, aStar * invNumPx, bStar * invNumPx);
}

size_t findClosestColorIdx(
    clrspc::Lab const& targetColor, std::vector<clrspc::Lab> const& quantColors)
{
    size_t closestColorIdx = 0;
    float minDist = std::numeric_limits<float>::max();

    for (size_t i = 0; i < quantColors.size(); ++i) {
        float const currDist = distSquared(targetColor, quantColors[i]); // fast
        // const float currDist = targetColor.diff_cie_2000(quantColors[i]); //
        // slow

        if (currDist < minDist) {
            minDist = currDist;
            closestColorIdx = i;
        }
    }

    return closestColorIdx;
}

std::vector<std::vector<int>> buildLookupTable(
    Bitmap const& bitmap, std::vector<clrspc::Lab> const& quantColors)
{
    Timer timer("buildLookupTable");
    std::vector<std::vector<int>> lookupTable(bitmap.m_height, std::vector<int>(bitmap.m_width));

    for (int i = 0; i < bitmap.m_width; i++) {
        for (int j = 0; j < bitmap.m_height; j++) {

            int const texIdx = findClosestColorIdx(bitmap.get(i, j).to_lab(), quantColors);

            lookupTable[j][i] = texIdx;
        }
    }

    return lookupTable;
}

void saveAsPNG(Bitmap const& bitmap)
{
    Picture quantPic(bitmap.m_width, bitmap.m_height, 0, 0, 0);

    for (int i = 0; i < bitmap.m_width; i++) {
        for (int j = 0; j < bitmap.m_height; j++) {
            auto [r, g, b] = bitmap.get(i, j).get_values();

            quantPic.set(i, j, r, g, b);
        }
    }

    quantPic.save("./outputPics/quantizedPic.png");
}
