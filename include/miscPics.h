#ifndef MISC_PICS_H
#define MISC_PICS_H

#include "../include/picture.h"

#include <vector>

void createQuantizedPic(const BitMap &bitMap);

void createAtlasPic(const std::vector<BitMap> &validTextures);

#endif
