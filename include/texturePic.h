#pragma once

#include <string>
#include <vector>

#include "../include/Bitmap.h"
#include "../include/util.h"

void getTextureData(std::vector<Bitmap> &validTextures,
                    std::vector<clrspc::Lab> &textureAvgColors);

void createTexturedPic(const std::vector<std::vector<int>> &textureLookupTable,
                       const std::vector<Bitmap> &validTextures);
