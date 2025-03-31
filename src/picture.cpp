
#include "../include/picture.h"

#include <algorithm>
#include <exception>
#include <stdexcept>
#include <string>
#include <vector>


Picture::Picture(int width, int height, int red, int green, int blue, int alpha)
    : _values(width * height * 4) {
  _width = width;
  _height = height;
  for (size_t k = 0; k < _values.size(); k += 4) {
    _values[k] = red;
    _values[k + 1] = green;
    _values[k + 2] = blue;
    _values[k + 3] = alpha;
  }
}


Picture::Picture(const std::vector<std::vector<int>> &grays) {
  if (grays.size() == 0 || grays[0].size() == 0) {
    _width = 0;
    _height = 0;
  } else {
    _values = std::vector<unsigned char>(grays[0].size() * grays.size() * 4);
    _width = grays[0].size();
    _height = grays.size();
    int k = 0;
    for (int y = 0; y < _height; y++)
      for (int x = 0; x < _width; x++) {
        int gray = grays[y][x];
        _values[k] = gray;
        _values[k + 1] = gray;
        _values[k + 2] = gray;
        _values[k + 3] = gray;
        k += 4;
      }
  }
}


void Picture::save(const std::string &filename) const {
  unsigned error = lodepng::encode(filename.c_str(), _values, _width, _height);
  if (error != 0)
    throw std::runtime_error(lodepng_error_text(error));
}


int Picture::red(int x, int y) const {
  if (0 <= x && x < _width && 0 <= y && y < _height)
    return _values[4 * (y * _width + x)];
  else
    return 0;
}


int Picture::green(int x, int y) const {
  if (0 <= x && x < _width && 0 <= y && y < _height)
    return _values[4 * (y * _width + x) + 1];
  else
    return 0;
}


int Picture::blue(int x, int y) const {
  if (0 <= x && x < _width && 0 <= y && y < _height)
    return _values[4 * (y * _width + x) + 2];
  else
    return 0;
}


int Picture::alpha(int x, int y) const {
  if (0 <= x && x < _width && 0 <= y && y < _height)
    return _values[4 * (y * _width + x) + 3];
  else
    return 0;
}


void Picture::set(int x, int y, int red, int green, int blue, int alpha) {
  if (x >= 0 && y >= 0) {
    ensure(x, y);
    int k = 4 * (y * _width + x);
    _values[k] = red;
    _values[k + 1] = green;
    _values[k + 2] = blue;
    _values[k + 3] = alpha;
  }
}


Picture::Picture(const std::string &filename) {
  unsigned int w, h;
  unsigned error = lodepng::decode(_values, w, h, filename);
  if (error != 0)
    throw std::runtime_error(lodepng_error_text(error));
  _width = w;
  _height = h;
}


void Picture::ensure(int x, int y) {
  if (x >= _width || y >= _height) {
    int new_width = std::max(x + 1, _width);
    int new_height = std::max(y + 1, _height);
    std::vector<unsigned char> new_values(4 * new_width * new_height);
    fill(new_values.begin(), new_values.end(), 255); // fill with white
    int j = 0;
    for (int dy = 0; dy < _height; dy++)
      for (int dx = 0; dx < _width; dx++)
        for (int k = 0; k < 4; k++, j++)
          new_values[4 * (dy * new_width + dx) + k] = _values[j];
    _values.swap(new_values);
    _width = new_width;
    _height = new_height;
  }
}
