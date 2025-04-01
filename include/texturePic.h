#ifndef TEXTURE_PIC_H
#define TEXTURE_PIC_H

#include <string>
#include <vector>

#include "../include/picture.h"
#include "../include/util.h"

std::vector<std::string> getValidPaths(std::string dir);

std::vector<BitMap> getValidTextures(std::vector<std::string> fPaths);

void createTexturedPic(const BitMap &bitMap,
                       const std::vector<BitMap> &validTextures);

#endif
