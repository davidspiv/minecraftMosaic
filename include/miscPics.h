#ifndef MISC_PICS_H
#define MISC_PICS_H

#include "../include/picture.h"

#include <vector>

void createQuantizedPic(const Picture &srcPic);

void createAtlasPic(const std::vector<Picture> &validTextures);

#endif
