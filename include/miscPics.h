#ifndef MISC_PICS_H
#define MISC_PICS_H

#include "../include/picture.h"

#include <vector>

void createQuantizedPic(const Bitmap &bitmap);

void createAtlasPic(const std::vector<Bitmap> &validTextures);

#endif
