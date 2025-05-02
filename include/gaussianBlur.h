#pragma once

#include "picture.h"
#include <iostream>

//  A gaussian kernel is a separable matrix, which means we can use its products
//  to perform two separate convolutions for each pixel. We then multiply the
//  results of both operations together to get an equivalent result, saving a
//  4th inner loop and a bunch of multiplication. Modifies picture object.
void gaussianBlur(Picture &pic, const size_t strength);
