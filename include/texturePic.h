#pragma once

#include <string>
#include <vector>

#include "../include/Bitmap.h"
#include "../include/util.h"

std::vector<std::string> getValidPaths(const std::string &dir);

std::vector<Bitmap> getValidTextures(const std::vector<std::string> &fPaths);

void createTexturedPic(const std::vector<std::vector<int>> &textureLookupTable,
                       const std::vector<Bitmap> &validTextures);

std::vector<clrspc::Lab>
getTextureAvgColors(const std::vector<Bitmap> &validTextures);
