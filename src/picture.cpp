
#include "../include/picture.h"
#include "../include/Color_Space.h"
#include "../include/timer.h"

#include <algorithm>
#include <cmath>
#include <cstring>
#include <exception>
#include <iostream>
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


// Picture::Picture(const Bitmap &bitmap) {
//   _width = bitmap.width();
//   _height = bitmap.height();

//   _values.resize(_width * _height * 4);

//   unsigned char *ptr = _values.data();

//   for (int j = 0; j < _height; j++) {
//     for (int i = 0; i < _width; i++) {
//       auto [r, g, b] = clrspc::Rgb(bitmap.get(i, j));

//       *ptr++ = r;
//       *ptr++ = g;
//       *ptr++ = b;
//       *ptr++ = 255;
//     }
//   }
// }


Picture::Picture(const Bitmap &bitmap, const int factor) {
  const int channels = 4;

  _width = bitmap.width() * factor;
  _height = bitmap.height() * factor;

  // Allocate enough space for the expanded image
  _values.resize(_width * _height * channels);

  for (int j = 0; j < bitmap.height(); j++) {
    for (int i = 0; i < bitmap.width(); i++) {
      auto [r, g, b] = bitmap.get(i, j).to_xyz().to_rgb().get_values();

      // Write the scaled pixels
      for (int y = 0; y < factor; y++) {
        for (int x = 0; x < factor; x++) {
          size_t dstIdx =
              ((j * factor + y) * _width + (i * factor + x)) * channels;
          _values[dstIdx] = r;
          _values[dstIdx + 1] = g;
          _values[dstIdx + 2] = b;
          _values[dstIdx + 3] = 255; // Alpha channel
        }
      }
    }
  }
}


Bitmap Picture::getBitmap() const {
  Bitmap bitmap(_width, _height);

  for (int y = 0; y < _height; ++y) {
    for (int x = 0; x < _width; ++x) {
      int i = (y * _width + x) * 4;

      const int r = _values[i];
      const int g = _values[i + 1];
      const int b = _values[i + 2];

      bitmap.set(x, y, clrspc::Rgb(r, g, b).to_xyz().to_lab());
    }
  }

  return bitmap;
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

int clampVal(double val) { return std::clamp(int(std::round(val)), 0, 255); };


Picture Picture::bilinearResize(double factor) const {
  if (factor == 1)
    return *this;

  // returns a rgb struct not associated with the ImageEditor class.
  auto getRgb = [&](int x, int y) -> const clrspc::Rgb {
    return {red(x, y), green(x, y), blue(x, y)};
  };

  const size_t inHeight = _height;
  const size_t inWidth = _width;
  const size_t outHeight = static_cast<int>(round(inHeight * factor));
  const size_t outWidth = static_cast<int>(round(inWidth * factor));

  const double xRatio = outWidth > 1 ? double(inWidth - 1) / (outWidth - 1) : 0;
  const double yRatio =
      outHeight > 1 ? double(inHeight - 1) / (outHeight - 1) : 0;

  Picture newPic(outWidth, outHeight, 0, 0, 0);

  for (size_t i = 0; i < outHeight; i++) {
    for (size_t j = 0; j < outWidth; j++) {
      const int yLow = std::floor(yRatio * i);
      const int xLow = std::floor(xRatio * j);
      const int xHigh = std::min(xLow + 1, int(inWidth - 1));
      const int yHigh = std::min(yLow + 1, int(inHeight - 1));

      const double yWeight = yRatio * i - yLow;
      const double xWeight = xRatio * j - xLow;

      // A,B,C, and D are known rgb values in original image
      clrspc::Rgb A = getRgb(xLow, yLow);
      clrspc::Rgb B = getRgb(xHigh, yLow);
      clrspc::Rgb C = getRgb(xLow, yHigh);
      clrspc::Rgb D = getRgb(xHigh, yHigh);

      // computes a weighted average of the values associated with the four
      // closest points
      auto interpolate = [xWeight, yWeight](float a, float b, float c,
                                            float d) {
        // We first compute the interpolated value of AB and CD in the width
        // dimension
        const float interpolatedAB = a * (1 - xWeight) + b * xWeight;
        const float interpolatedCD = c * (1 - xWeight) + d * xWeight;

        // Then we will do linear interpolation between the points generated
        // from the two previous interpolations above
        return (interpolatedAB * (1.f - yWeight)) + (interpolatedCD * yWeight);
      };

      auto calcRgb = [&](const clrspc::Rgb &A, const clrspc::Rgb &B,
                         const clrspc::Rgb &C,
                         const clrspc::Rgb &D) -> clrspc::Rgb {
        return {interpolate(A.r(), B.r(), C.r(), D.r()),
                interpolate(A.g(), B.g(), C.g(), D.g()),
                interpolate(A.b(), B.b(), C.b(), D.b())};
      };

      const auto [r, g, b] = calcRgb(A, B, C, D).get_values();

      newPic.set(j, i, r, g, b);
    }
  }

  return newPic;
};


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
