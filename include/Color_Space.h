#pragma once

#include <algorithm>
#include <array>
#include <cmath>
#include <iostream>
#include <string>
#include <vector>

#define _USE_MATH_DEFINES // for VS

namespace clrspc {

// forward declarations
class Lab;
class Rgb;

class Color {
protected:
  std::array<float, 3> m_values;

public:
  Color(float x, float y, float z) { m_values = {x, y, z}; }

  [[nodiscard]] std::array<float, 3> get_values() const { return m_values; }

  virtual void print() const {
    std::cout << "[GEN]" << "\nC1: " << m_values[0] << "\nC2: " << m_values[1]
              << "\nC3: " << m_values[2] << "\n\n";
  }

  [[nodiscard]] bool operator==(const Color &other) const {
    auto [x, y, z] = m_values;
    float error = 1;
    auto [other_x, other_y, other_z] = other.get_values();
    return (std::abs(x - other_x) < error) && (std::abs(y - other_y) < error) &&
           (std::abs(z - other_z) < error);
  }

  [[nodiscard]] bool operator!=(const Color &other) const {
    return !(*this == other);
  }
};


class Lab : public Color {
public:
  Lab(float l, float a, float b);

  // accessors
  inline float l() const { return m_values[0]; }
  inline float a() const { return m_values[1]; }
  inline float b() const { return m_values[2]; }

  [[nodiscard]] Rgb to_rgb() const;

  void print() const override;
};


class Rgb : public Color {
public:
  Rgb(float r, float g, float b);

  // accessors
  inline float r() const { return m_values[0]; }
  inline float g() const { return m_values[1]; }
  inline float b() const { return m_values[2]; }

  [[nodiscard]] Lab to_lab() const;

  void print() const override;
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


} // namespace clrspc
