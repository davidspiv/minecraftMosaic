#pragma once

#include "Color_Space.h"

class Bitmap {
public:
  Bitmap(int width, int height)
      : m_width(width), m_height(height),
        m_bits(height,
               std::vector<clrspc::Rgb>(width, clrspc::Rgb(0l, 0l, 0l))) {}

  int m_width;
  int m_height;
  std::vector<std::vector<clrspc::Rgb>> m_bits;
};
