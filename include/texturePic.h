#pragma once

#include <string>
#include <vector>

#include "../include/picture.h"
#include "../include/util.h"

std::vector<std::string> getValidPaths(std::string dir);

std::vector<Bitmap> getValidTextures(std::vector<std::string> fPaths);

void createTexturedPic(const Bitmap &bitmap,
                       const std::vector<Bitmap> &validTextures);

