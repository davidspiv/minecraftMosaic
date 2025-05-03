#pragma once

#include "Color_Space.h"

#include <cstring>

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

  std::array<uchar, 64> getSixteen(int x, int y) const {
    std::array<uchar, 64> outArr;

    for (int i = 0; i < 16; ++i) {
      const int index = CHANNELS * (y * m_width + x + i);
      outArr[i * 4 + 0] = m_bits[index + 0]; // R
      outArr[i * 4 + 1] = m_bits[index + 1]; // G
      outArr[i * 4 + 2] = m_bits[index + 2]; // B
      outArr[i * 4 + 3] = 255;               // A
    }

    return outArr;
  }

  int m_width;
  int m_height;
  std::vector<uchar> m_bits;
};
