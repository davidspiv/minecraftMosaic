#ifndef TEXTURE_PIC_H
#define TEXTURE_PIC_H

#include <string>
#include <vector>

#include "../include/picture.h"
#include "../include/util.h"

std::vector<std::string> getValidPaths(std::string dir);

std::vector<Picture> getValidTextures(std::vector<std::string> fPaths);

std::vector<StdRGB>
getTextureAvgColors(const std::vector<Picture> &validTextures);

void createTexturedPic(const Picture &srcPic,
                       const std::vector<Picture> &validTextures);

#endif
