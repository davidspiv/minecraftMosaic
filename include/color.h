#ifndef COLOR_H
#define COLOR_H

#include <iostream>
#include <stdexcept>
#include <vector>

struct CieLab;
struct LinRGB;

struct StdRGB {
  StdRGB() : r(255), g(255), b(255) {}
  StdRGB(int r, int g, int b);
  explicit StdRGB(const CieLab &ceiLab);
  explicit StdRGB(const LinRGB &linRGB);
  int r, g, b;
};

struct LinRGB {
  LinRGB() : r(255.0), g(255.0), b(255.0) {}
  LinRGB(double r, double g, double b);
  explicit LinRGB(const StdRGB &stdRgb);
  double r, g, b;
};


struct CieXYZ {
  CieXYZ() : x(0), y(0), z(0) {}
  CieXYZ(double x, double y, double z);
  double x, y, z;
};

struct CieLab {
  CieLab() : lStar(0), aStar(0), bStar(0) {}
  CieLab(double lStar, double aStar, double bStar);
  explicit CieLab(const StdRGB &stdRgb);
  double lStar, aStar, bStar;
};

class Bitmap {
public:
  Bitmap(int width, int height)
      : _width(width), _height(height),
        bits(height, std::vector<CieLab>(width, CieLab())) {}

  CieLab get(int x, int y) const {
    if (x < 0 || x >= _width || y < 0 || y >= _height) {
      std::cout << "x: " << x << "y: " << y << std::endl;
      throw std::out_of_range("Out of range");
    }
    return bits[y][x];
  }

  StdRGB getRBG(int x, int y) const {
    if (x < 0 || x >= _width || y < 0 || y >= _height) {
      std::cout << "x: " << x << "y: " << y << std::endl;
      throw std::out_of_range("Out of range");
    }
    const StdRGB stdRGB(bits[y][x]);

    return stdRGB;
  }

  void set(int x, int y, const CieLab &value) {
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
  std::vector<std::vector<CieLab>> bits;
};

const CieXYZ referenceWhiteD60(0.950470, 1.0, 1.088830);
constexpr double epsilon = 216.0 / 24389.0;
constexpr double kappa = 24389.0 / 27.0;

LinRGB linearize(const StdRGB &stdRGB);
StdRGB applyGamma(const LinRGB &linRGB);
CieXYZ rgbToXYZ(const LinRGB &linRGB);
LinRGB xyzToRGB(const CieXYZ &cieXYZ);
CieLab xyzToLab(const CieXYZ &cieXYZ);
CieXYZ labToXYZ(const CieLab &cieLab);

#endif
