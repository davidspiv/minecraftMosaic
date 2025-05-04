#pragma once

#include <string>
#include <vector>

#include "../include/Bitmap.h"
#include "../include/util.h"

std::vector<std::string> getPaths(const std::string &dir);

std::vector<std::string> getValidPaths(const std::vector<std::string> &paths);

std::vector<Bitmap> getValidTextures(const std::vector<std::string> &fPaths);

std::vector<clrspc::Lab>
calcTextureAvgColors(const std::vector<Bitmap> &validTextures);

void getTextureData(std::vector<Bitmap> &validTextures,
                    std::vector<clrspc::Lab> &textureAvgColors);

void createTexturedPic(const std::vector<std::vector<int>> &textureLookupTable,
                       const std::vector<Bitmap> &validTextures);
