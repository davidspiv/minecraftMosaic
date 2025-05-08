#pragma once

#include <algorithm>
#include <array>
#include <cmath>
#include <iostream>
#include <string>
#include <vector>

#define _USE_MATH_DEFINES // for VS

using uchar = std::uint8_t;

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

  virtual void print() const = 0;

  [[nodiscard]] bool operator==(Color const &other) const {
    auto [x, y, z] = m_values;
    float error = 1;
    auto [other_x, other_y, other_z] = other.get_values();
    return (std::abs(x - other_x) < error) && (std::abs(y - other_y) < error) &&
           (std::abs(z - other_z) < error);
  }

  [[nodiscard]] bool operator!=(Color const &other) const {
    return !(*this == other);
  }
};

class Lab : public Color {
public:
  Lab(float l, float a, float b);

  float l() const { return m_values[0]; }
  float a() const { return m_values[1]; }
  float b() const { return m_values[2]; }

  [[nodiscard]] Rgb to_rgb() const;

  void print() const override;
};

class Rgb : public Color {
public:
  Rgb(float r, float g, float b);

  float r() const { return m_values[0]; }
  float g() const { return m_values[1]; }
  float b() const { return m_values[2]; }

  [[nodiscard]] Lab to_lab() const;

  void print() const override;
};

// =========== Utils ==========

inline float to_radians(float const degrees) {
  return degrees * (M_PI / 180.f);
}

inline float to_degrees(float const radians) {
  return radians * (180.f / M_PI);
}

inline float normalize_degrees(float x) {
  return x - std::floor(x / 360.0f) * 360.0f;
}

// =========== okLAB Space ==========

inline Lab::Lab(float l, float a, float b) : Color(l, a, b) {}

inline Rgb Lab::to_rgb() const

{
  auto normal_gamma = [](float c) -> uchar {
    c = std::fmax(0.f, c);
    float encoded =
        (c <= 0.0031308f)
            ? 12.92f * c
            : 1.055f * std::exp2f(std::log2f(c) * 0.41666f) - 0.055f;
    return static_cast<uchar>(encoded * 255.f);
  };

  auto [L, a, b] = m_values;

  float l_ = L + 0.3963377774f * a + 0.2158037573f * b;
  float m_ = L - 0.1055613458f * a - 0.0638541728f * b;
  float s_ = L - 0.0894841775f * a - 1.2914855480f * b;

  float l = l_ * l_ * l_;
  float m = m_ * m_ * m_;
  float s = s_ * s_ * s_;

  float r1 = +4.0767416621f * l - 3.3077115913f * m + 0.2309699292f * s;
  float g1 = -1.2684380046f * l + 2.6097574011f * m - 0.3413193965f * s;
  float b1 = -0.0041960863f * l - 0.7034186147f * m + 1.7076147010f * s;

  return {normal_gamma(r1), normal_gamma(g1), normal_gamma(b1)};
}

inline void Lab::print() const {
  std::cout << "[Lab]" << "\nL: " << m_values[0] << "\na: " << m_values[1]
            << "\nb: " << m_values[2] << "\n\n";
}

// ========== sRGB Space ==========

inline Rgb::Rgb(float r, float g, float b)
    : Color{static_cast<uint8_t>(std::clamp(std::lround(r), 0l, 255l)),
            static_cast<uint8_t>(std::clamp(std::lround(g), 0l, 255l)),
            static_cast<uint8_t>(std::clamp(std::lround(b), 0l, 255l))} {

  auto warn_if_clamped = [](float val, const char *name) {
    if (val < -0.001f || val > 255.001f) {
      std::cout << "Warning: " << name << " channel clamped: " << val << '\n';
    }
  };

  warn_if_clamped(r, "R");
  warn_if_clamped(g, "G");
  warn_if_clamped(b, "B");
}

inline Lab Rgb::to_lab() const {
  auto normal_linear = [](uchar c) -> float {
    float cf = std::fmax(0.f, static_cast<float>(c) / 255.f);
    return (cf <= 0.04045f) ? cf / 12.92f
                            : std::pow((cf + 0.055f) / 1.055f, 2.4f);
  };

  float r_lin = normal_linear(r());
  float g_lin = normal_linear(g());
  float b_lin = normal_linear(b());

  float l =
      0.4122214708f * r_lin + 0.5363325363f * g_lin + 0.0514459929f * b_lin;
  float m =
      0.2119034982f * r_lin + 0.6806995451f * g_lin + 0.1073969566f * b_lin;
  float s =
      0.0883024619f * r_lin + 0.2817188376f * g_lin + 0.6299787005f * b_lin;

  float l_ = cbrtf(l);
  float m_ = cbrtf(m);
  float s_ = cbrtf(s);

  return {
      0.2104542553f * l_ + 0.7936177850f * m_ - 0.0040720468f * s_,
      1.9779984951f * l_ - 2.4285922050f * m_ + 0.4505937099f * s_,
      0.0259040371f * l_ + 0.7827717662f * m_ - 0.8086757660f * s_,
  };
}

inline void Rgb::print() const {
  std::cout << "[Rgb]" << "\nr: " << (int)m_values[0]
            << "\ng: " << (int)m_values[1] << "\nb: " << (int)m_values[2]
            << std::endl;
}

} // namespace clrspc
