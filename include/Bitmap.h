#pragma once

#include "Color_Space.h"

using uchar = std::uint8_t;

class Bitmap {
public:
  static const int CHANNELS = 3;

  Bitmap(int width, int height)
      : m_width(width), m_height(height),
        m_bits(std::vector<uchar>(CHANNELS * width * height)) {}

  void set(int x, int y, const clrspc::Rgb &rgb) {
    const auto [r, g, b] = rgb.get_values();
    m_bits[CHANNELS * (y * m_width + x)] = r;
    m_bits[CHANNELS * (y * m_width + x) + 1] = g;
    m_bits[CHANNELS * (y * m_width + x) + 2] = b;
  }

  const clrspc::Rgb get(int x, int y) const {
    const uchar r = m_bits[CHANNELS * (y * m_width + x)];
    const uchar g = m_bits[CHANNELS * (y * m_width + x) + 1];
    const uchar b = m_bits[CHANNELS * (y * m_width + x) + 2];

    return clrspc::Rgb(r, g, b);
  }

  int m_width;
  int m_height;
  std::vector<uchar> m_bits;
};
