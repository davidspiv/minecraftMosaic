#ifndef PICTURE_H
#define PICTURE_H

#include "Bitmap.h"
#include "lodepng.h"

#include <string>
#include <vector>


class Picture {
public:
  explicit Picture(const std::string &filename);
  explicit Picture(const std::vector<std::vector<int>> &grays);
  Picture(const Bitmap &bitmap, const int factor = 1);
  Picture(int width, int height, int red = 0, int green = 0, int blue = 0,
          int alpha = 255);

  int width() const { return _width; }
  int height() const { return _height; }
  int red(int x, int y) const;
  int green(int x, int y) const;
  int blue(int x, int y) const;
  int alpha(int x, int y) const;

  void set(int x, int y, int red, int green, int blue, int alpha = 255);
  void save(const std::string &filename) const;
  Picture bilinearResize(double factor) const;
  Bitmap getBitmap() const;

private:
  void ensure(int x, int y);

  std::vector<unsigned char> _values;
  int _width;
  int _height;
};

#endif
