#pragma once

#include <algorithm>
#include <array>
#include <cmath>
#include <iostream>
#include <string>
#include <vector>

#include "Matrix.h"

#define _USE_MATH_DEFINES // for VS

namespace clrspc {

static constexpr float epsilon = 216.0f / 24389.0f;
static constexpr float kappa = 24389.0f / 27.0f;
const std::array<float, 3> REF_WHITE_D65 = {0.95047f, 1.00000f, 1.08883f};

// forward declarations
class Lab;
class Lch_Ab;
class Rgb;
class Xyz;

class Color {
protected:
  std::array<float, 3> m_values;

public:
  Color(float x, float y, float z);

  /**
   * @brief Returns the color's internal channel values.
   * @return Array containing 3 float components
   */
  [[nodiscard]] std::array<float, 3> get_values() const;

  /**
   * @brief Returns the color as a 3x1 column matrix representation.
   * @return Matrix with column form of color values.
   */
  [[nodiscard]] Matrix to_column() const;

  virtual void print() const;

  [[nodiscard]] bool operator==(const Color &other) const;
  [[nodiscard]] bool operator!=(const Color &other) const;
};


class Lab : public Color {
public:
  /**
   * @brief Constructs a Lab color.
   * @param l Lightness (L*)
   * @param a Green–Red component (a*)
   * @param b Blue–Yellow component (b*)
   */
  Lab(float l, float a, float b);

  // accessors
  inline float l() const { return m_values[0]; }
  inline float a() const { return m_values[1]; }
  inline float b() const { return m_values[2]; }

  /**
   * @brief Converts Lab to XYZ color space.
   * @return the converted color as a Xyz object
   */
  Xyz to_xyz() const;

  /**
   * @brief Converts Lab to cylindrical Lch(ab) format.
   * @return the converted color as a Lch_Ab object
   */
  Lch_Ab to_lch_ab() const;

  /**
   * @brief Prints Lab components to the console.
   */
  void print() const override;
};


class Lch_Ab : public Color {
public:
  /**
   * @brief Constructs an Lch(ab) color (cylindrical Lab).
   * @param l Lightness
   * @param c Chroma
   * @param h Hue angle [degrees]
   */
  Lch_Ab(float l, float c, float h);

  [[nodiscard]] Lab to_lab() const;

  /**
   * @brief Prints Lch(ab) components to the console.
   */
  void print() const override;
};


class Rgb : public Color {
public:
  /**
   * @brief Constructs an RGB color.
   * @param r Red channel
   * @param g Green channel
   * @param b Blue channel
   */
  Rgb(float r, float g, float b);

  // accessors
  inline float r() const { return m_values[0]; }
  inline float g() const { return m_values[1]; }
  inline float b() const { return m_values[2]; }

  /**
   * @brief Converts RGB to XYZ color space.
   * @param profile The RGB color space (sRGB, Adobe RGB, etc.)
   * @return the converted color as a Xyz object
   */
  [[nodiscard]] Xyz to_xyz() const;

  /**
   * @brief Prints RGB components to the console.
   */
  void print() const override;
};


class Xyz : public Color {
public:
  /**
   * @brief Constructs an XYZ color.
   * @param X a mix of the three CIE RGB curves chosen to be nonnegative (X)
   * @param Y luminance (Y)
   * @param Z quasi-equal to the "blue" monochromatic primary (Z)
   */
  Xyz(float X, float Y, float Z);

  /**
   * @brief Converts XYZ to RGB.
   * @param profile RGB color space to use for conversion.
   * @return RGB color.
   */
  [[nodiscard]] Rgb to_rgb() const;

  /**
   * @brief Converts XYZ to Lab.
   * @return the converted color as a Lab object
   */
  [[nodiscard]] Lab to_lab() const;

  /**
   * @brief Prints XYZ components to the console.
   */
  void print() const;
};


// =========== Utils ==========

inline float to_radians(const float degrees) {
  return degrees * (M_PI / 180.f);
}


inline float to_degrees(const float radians) {
  return radians * (180.0 / M_PI);
}


inline float remove_gamma(float c) {
  if (c <= 0) {
    return c;
  }

  return (c <= 0.04045f) ? (c / 12.92f) : std::pow((c + 0.055f) / 1.055f, 2.4f);
}


inline float apply_gamma(const float c) {
  if (c <= 0) {
    return c;
  }

  return (c <= 0.0031308f) ? (c * 12.92f)
                           : 1.055f * std::pow(c, 1.0f / 2.4f) - 0.055f;
}


inline double normalize_degrees(double x) {
  return x - std::floor(x / 360.0f) * 360.0f;
}


std::array<float, 3> inline to_polar_color_space(
    const std::array<float, 3> &cartesianColor_Space) {
  const auto [l, a, b] = cartesianColor_Space; // LchAb equivalents: a=u and b=v
  const float c = std::sqrt(a * a + b * b);
  const float h_component = to_degrees(std::atan2(b, a));
  const float h = (h_component >= 0) ? h_component : h_component + 360.0;

  return {l, c, h};
}


}
