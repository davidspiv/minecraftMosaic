#pragma once

#include "Color_Space.h"

class Bitmap {
public:
  Bitmap(int width, int height)
      : m_width(width), m_height(height),
        m_bits(std::vector<clrspc::Rgb>(width * height,
                                        clrspc::Rgb(0l, 0l, 0l))) {}

  void set(int x, int y, const clrspc::Rgb &rgb) {
    m_bits[y * m_width + x] = rgb;
  }
  clrspc::Rgb get(int x, int y) const { return m_bits[y * m_width + x]; }

  int m_width;
  int m_height;
  std::vector<clrspc::Rgb> m_bits;
};
