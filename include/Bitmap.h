#pragma once

#include "Color_Space.h"

class Bitmap {
public:
  Bitmap(int width, int height)
      : _width(width), _height(height),
        bits(height,
             std::vector<clrspc::Lab>(width, clrspc::Lab(0.f, 0.f, 0.f))) {}

  clrspc::Lab get(int x, int y) const {
    if (x < 0 || x >= _width || y < 0 || y >= _height) {
      std::cout << "x: " << x << "y: " << y << std::endl;
      throw std::out_of_range("Out of range");
    }
    return bits[y][x];
  }

  clrspc::Rgb getRBG(int x, int y) const {
    if (x < 0 || x >= _width || y < 0 || y >= _height) {
      std::cout << "x: " << x << "y: " << y << std::endl;
      throw std::out_of_range("Out of range");
    }
    const clrspc::Rgb rgb = bits[y][x].to_xyz().to_rgb();

    return rgb;
  }

  void set(int x, int y, const clrspc::Lab &value) {
    if (x < 0 || x >= _width || y < 0 || y >= _height) {
      std::cout << "x: " << x << "y: " << y << std::endl;
      throw std::out_of_range("Out of range");
    }
    bits[y][x] = value;
  }

  int width() const { return _width; }

  int height() const { return _height; }

private:
  int _width;
  int _height;
  std::vector<std::vector<clrspc::Lab>> bits;
};
