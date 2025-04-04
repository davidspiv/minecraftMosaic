#include "../include/color.h"
#include "../include/util.h"

#include <algorithm>
#include <array>
#include <cmath>
#include <iostream>


StdRGB::StdRGB(int r, int g, int b) : r(r), g(g), b(b) {
  auto validate = [](int c) {
    if (std::min(255, std::max(0, c)) != c) {
      throw std::domain_error("Channel initalized outside of range [0, 255].");
    }
  };

  validate(r);
  validate(g);
  validate(b);
};


StdRGB::StdRGB(const CieLab &cieLab) {
  CieXYZ cieXYZ = labToXYZ(cieLab);
  LinRGB linRGB = xyzToRGB(cieXYZ);
  auto [r, g, b] = applyGamma(linRGB);

  this->r = r;
  this->g = g;
  this->b = b;
};


StdRGB::StdRGB(const LinRGB &linRGB) {
  auto [r, g, b] = applyGamma(linRGB);

  this->r = r;
  this->g = g;
  this->b = b;
};


LinRGB::LinRGB(double r, double g, double b) : r(r), g(g), b(b) {
  auto validate = [](double c) {
    if (std::min(1.0, std::max(0.0, c)) != c) {
      throw std::domain_error("Channel initalized outside of range [0, 1].");
    }
  };

  validate(r);
  validate(g);
  validate(b);
};


LinRGB::LinRGB(const StdRGB &stdRgb) {
  auto [r, g, b] = linearize(stdRgb);

  this->r = r;
  this->g = g;
  this->b = b;
};


CieXYZ::CieXYZ(double x, double y, double z) : x(x), y(y), z(z) {};


CieLab::CieLab(double lStar, double aStar, double bStar)
    : lStar(lStar), aStar(aStar), bStar(bStar) {};


CieLab::CieLab(const StdRGB &stdRgb) {
  LinRGB linRGB = linearize(stdRgb);
  CieXYZ cieXYZ = rgbToXYZ(linRGB);
  auto [lStar, aStar, bStar] = xyzToLab(cieXYZ);

  this->lStar = lStar;
  this->aStar = aStar;
  this->bStar = bStar;
};


LinRGB linearize(const StdRGB &stdRGB) {
  // linear lookup table
  static std::array<double, 256> linearLUT = [] {
    std::array<double, 256> lut{};
    for (int i = 0; i < 256; ++i) {
      double normalized = i / 255.0;
      lut[i] = (normalized <= 0.04045)
                   ? (normalized / 12.92)
                   : std::pow((normalized + 0.055) / 1.055, 2.4);
    }
    return lut;
  }();


  return {linearLUT[stdRGB.r], linearLUT[stdRGB.g], linearLUT[stdRGB.b]};
}


StdRGB applyGamma(const LinRGB &linRGB) {
  // gamma lookup table
  static std::array<uint8_t, 256> gammaLUT = [] {
    std::array<uint8_t, 256> lut{};
    for (int i = 0; i < 256; ++i) {
      double linear = i / 255.0;
      double corrected = (linear <= 0.0031308)
                             ? (linear * 12.92)
                             : 1.055 * std::pow(linear, 1.0 / 2.4) - 0.055;
      lut[i] = static_cast<uint8_t>(std::round(corrected * 255.0));
    }
    return lut;
  }();


  return {gammaLUT[std::clamp(static_cast<int>(std::round(linRGB.r * 255.0)), 0,
                              255)],
          gammaLUT[std::clamp(static_cast<int>(std::round(linRGB.g * 255.0)), 0,
                              255)],
          gammaLUT[std::clamp(static_cast<int>(std::round(linRGB.b * 255.0)), 0,
                              255)]};
}


CieXYZ rgbToXYZ(const LinRGB &linRGB) {
  // reference white - D65
  constexpr std::array<std::array<double, 3>, 3> rgbToXYZMatrix = {
      {{0.4124564, 0.3575761, 0.1804375},
       {0.2126729, 0.7151522, 0.0721750},
       {0.0193339, 0.1191920, 0.9503041}}};

  std::array<double, 3> cieRGB =
      multiplyMatrix(rgbToXYZMatrix, {linRGB.r, linRGB.g, linRGB.b});

  return CieXYZ(cieRGB[0], cieRGB[1], cieRGB[2]);
}


LinRGB xyzToRGB(const CieXYZ &ceiLab) {
  // reference white - D65
  constexpr std::array<std::array<double, 3>, 3> xyzToRGBMatrix = {{
      {3.2404542, -1.5371385, -0.4985314},
      {-0.9692660, 1.8760108, 0.0415560},
      {0.0556434, -0.2040259, 1.0572252},
  }};

  std::array<double, 3> linRGB =
      multiplyMatrix(xyzToRGBMatrix, {ceiLab.x, ceiLab.y, ceiLab.z});

  const double r = std::clamp(linRGB[0], 0.0, 1.0);
  const double g = std::clamp(linRGB[1], 0.0, 1.0);
  const double b = std::clamp(linRGB[2], 0.0, 1.0);

  return LinRGB(r, g, b);
}


CieLab xyzToLab(const CieXYZ &cieXYZ) {
  auto [x, y, z] = cieXYZ;

  const double xR = x / referenceWhiteD60.x;
  const double yR = y / referenceWhiteD60.y;
  const double zR = z / referenceWhiteD60.z;

  auto f = [](double t) -> double {
    return (t > epsilon) ? std::cbrt(t) : (kappa * t + 16) / 116.0;
  };

  const double fX = f(xR);
  const double fY = f(yR);
  const double fZ = f(zR);

  return {(116.0 * fY) - 16.0, 500.0 * (fX - fY), 200.0 * (fY - fZ)};
}


CieXYZ labToXYZ(const CieLab &cieLab) {
  const double fY = (cieLab.lStar + 16) / 116.0;
  const double fX = fY + (cieLab.aStar / 500.0);
  const double fZ = fY - (cieLab.bStar / 200.0);

  // Compute inverse f() using a lambda
  auto fInv = [](double f) -> double {
    double f3 = f * f * f;
    return (f3 > epsilon) ? f3 : (116 * f - 16) / kappa;
  };

  const double xR = fInv(fX);
  const double yR =
      (cieLab.lStar > kappa * epsilon) ? std::pow(fY, 3) : cieLab.lStar / kappa;
  const double zR = fInv(fZ);

  return {xR * referenceWhiteD60.x, yR * referenceWhiteD60.y,
          zR * referenceWhiteD60.z};
}
