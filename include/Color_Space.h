#pragma once

#include <algorithm>
#include <array>
#include <cmath>
#include <iostream>
#include <string>
#include <vector>

#include "Matrix.h"

#define _USE_MATH_DEFINES // for VS

namespace Color_Space {

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

float to_radians(const float degrees) { return degrees * (M_PI / 180.f); }


float to_degrees(const float radians) { return radians * (180.0 / M_PI); }


float remove_gamma(float c) {
  if (c <= 0) {
    return c;
  }

  return (c <= 0.04045f) ? (c / 12.92f) : std::pow((c + 0.055f) / 1.055f, 2.4f);
}


float apply_gamma(const float c) {
  if (c <= 0) {
    return c;
  }

  return (c <= 0.0031308f) ? (c * 12.92f)
                           : 1.055f * std::pow(c, 1.0f / 2.4f) - 0.055f;
}


double normalize_degrees(double x) {
  return x - std::floor(x / 360.0f) * 360.0f;
}


std::array<float, 3>
to_polar_color_space(const std::array<float, 3> &cartesianColor_Space) {
  const auto [l, a, b] = cartesianColor_Space; // LchAb equivalents: a=u and b=v
  const float c = std::sqrt(a * a + b * b);
  const float h_component = to_degrees(std::atan2(b, a));
  const float h = (h_component >= 0) ? h_component : h_component + 360.0;

  return {l, c, h};
}


Color::Color(float x, float y, float z) { m_values = {x, y, z}; }


std::array<float, 3> Color::get_values() const { return m_values; }


void Color::print() const {
  std::cout << "[GEN]" << "\nC1: " << m_values[0] << "\nC2: " << m_values[1]
            << "\nC3: " << m_values[2] << "\n\n";
}


Matrix Color::to_column() const {
  return Matrix({{m_values[0]}, {m_values[1]}, {m_values[2]}});
};


bool Color::operator==(const Color &other) const {
  auto [x, y, z] = m_values;
  float error = 1;
  auto [other_x, other_y, other_z] = other.get_values();
  return (std::abs(x - other_x) < error) && (std::abs(y - other_y) < error) &&
         (std::abs(z - other_z) < error);
}


bool Color::operator!=(const Color &other) const { return !(*this == other); }


// =========== Lab Space ==========

Lab::Lab(float l, float a, float b) : Color(l, a, b) {}


Xyz Lab::to_xyz() const {
  const float fX = (m_values[0] + 16.0f) / 116.0f;
  const float fY = fX + (m_values[1] / 500.0f);
  const float fZ = fX - (m_values[2] / 200.0f);

  const float rX = (std::pow(fY, 3.0f) > epsilon)
                       ? std::pow(fY, 3)
                       : (116.0f * fY - 16.0f) / kappa;
  const float rY =
      (m_values[0] > (kappa * epsilon)) ? std::pow(fX, 3) : m_values[0] / kappa;
  const float rZ = (std::pow(fZ, 3.0f) > epsilon)
                       ? std::pow(fZ, 3)
                       : (116.0f * fZ - 16.0f) / kappa;

  auto [ref_x, ref_y, ref_z] = REF_WHITE_D65;

  const float x = rX * ref_x;
  const float y = rY * ref_y;
  const float z = rZ * ref_z;

  return Xyz(x, y, z);
}


Lch_Ab Lab::to_lch_ab() const {
  const auto [l, c, h] = to_polar_color_space(m_values);

  return Lch_Ab(l, c, h);
};


void Lab::print() const {
  std::cout << "[Lab]" << "\nL: " << m_values[0] << "\na: " << m_values[1]
            << "\nb: " << m_values[2] << "\n\n";
}


// ========= LCH(ab) Space =========

Lch_Ab::Lch_Ab(float l, float c, float h) : Color(l, c, h) {}


Lab Lch_Ab::to_lab() const {
  auto [l, c, h_deg] = m_values;
  const float h_rad = to_radians(h_deg);

  const float a = c * std::cos(h_rad);
  const float b = c * std::sin(h_rad);

  return Lab(l, a, b);
}


void Lch_Ab::print() const {
  std::cout << "[LCHab]" << "\nL: " << m_values[0] << "\nc: " << m_values[1]
            << "\nh: " << m_values[2] << "\n\n";
}


// ========== sRGB Space ==========

Rgb::Rgb(float r, float g, float b) : Color(r, g, b) {
  auto validate = [](float c) {
    if (std::min(255.0f, std::max(0.0f, c)) != c) {
      throw std::domain_error("Channel initalized outside of range [0, 255].");
    }
  };

  validate(r);
  validate(g);
  validate(b);
};


Xyz Rgb::to_xyz() const {

  auto [r, g, b] = m_values;

  // Step 1: Normalize input RGB [0–255] -> [0.0–1.0]
  const float r_lin = remove_gamma(r / 255.0f);
  const float g_lin = remove_gamma(g / 255.0f);
  const float b_lin = remove_gamma(b / 255.0f);

  // Step 2: Convert to XYZ using matrix
  const Matrix M_matrix = create_to_xyz_transformation_matrix(REF_WHITE_D65);
  const Matrix rgb_lin({{r_lin}, {g_lin}, {b_lin}});
  const Matrix xyz_matrix = M_matrix.multiply(rgb_lin);

  return Xyz(xyz_matrix(0, 0), xyz_matrix(1, 0), xyz_matrix(2, 0));
}


void Rgb::print() const {
  std::cout << "[Rgb]" << "\nr: " << m_values[0] << "\ng: " << m_values[1]
            << "\nb: " << m_values[2] << std::endl;
}


// ========== XYZ Space ===========

Xyz::Xyz(float x, float y, float z) : Color(x, y, z) {}


Rgb Xyz::to_rgb() const {

  const Matrix color_as_column = to_column();

  const Matrix M_matrix =
      create_to_xyz_transformation_matrix(REF_WHITE_D65).invert();

  Matrix xyz_as_matrix = M_matrix.multiply(color_as_column);

  // Absolute colorimetric
  const float r_corr = apply_gamma(xyz_as_matrix(0, 0));
  const float g_corr = apply_gamma(xyz_as_matrix(1, 0));
  const float b_corr = apply_gamma(xyz_as_matrix(2, 0));

  const float r_norm = std::clamp(r_corr, 0.0f, 1.0f) * 255.0f;
  const float g_norm = std::clamp(g_corr, 0.0f, 1.0f) * 255.0f;
  const float b_norm = std::clamp(b_corr, 0.0f, 1.0f) * 255.0f;

  return Rgb(r_norm, g_norm, b_norm);
}


Lab Xyz::to_lab() const {
  auto [w_X, w_Y, w_Z] = REF_WHITE_D65;

  const float xR = m_values[0] / w_X;
  const float yR = m_values[1] / w_Y;
  const float zR = m_values[2] / w_Z;

  const float fX = (xR > epsilon) ? std::cbrt(xR) : (kappa * xR + 16) / 116.0f;
  const float fY = (yR > epsilon) ? std::cbrt(yR) : (kappa * yR + 16) / 116.0f;
  const float fZ = (zR > epsilon) ? std::cbrt(zR) : (kappa * zR + 16) / 116.0f;

  const float l = 116.0f * fY - 16;
  const float a = 500.0f * (fX - fY);
  const float b = 200.0f * (fY - fZ);

  return Lab(l, a, b);
}


void Xyz::print() const {
  std::cout << "[XYZ]" << "\nX: " << m_values[0] << "\nY: " << m_values[1]
            << "\nZ: " << m_values[2] << "\n\n";
}

} // namespace Color_Space


/**
 * @brief Generates a smooth rainbow of colors starting from a specified color.
 *
 * @param sample_count The number of colors to generate. Must be at least 2.
 * @param start_color The color where the rainbow starts. Defaults to red.
 * @param rainbow_percent The portion of the color wheel to cover, as a
 * percentage (100 = full rainbow, 50 = half rainbow, etc.). Defaults to 100.
 *
 * @return std::vector<sf::Color> A list of colors smoothly transitioning across
 * the specified portion of the rainbow.
 */
