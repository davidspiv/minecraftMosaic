#include "../include/Color_Space.h"
#include "../include/Timer.h"
#include "../include/util.h"

namespace clrspc {


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
  const float fX = (m_values[0] + 16.f) / 116.f;
  const float fY = fX + (m_values[1] / 500.f);
  const float fZ = fX - (m_values[2] / 200.f);

  const float fY3 = fY * fY * fY;
  const float fX3 = fX * fX * fX;
  const float fZ3 = fZ * fZ * fZ;

  const float rX = (fY3 > epsilon) ? fY3 : (116.f * fY - 16.f) / kappa;
  const float rY =
      (m_values[0] > (kappa * epsilon)) ? fX3 : m_values[0] / kappa;
  const float rZ = (fZ3 > epsilon) ? fZ3 : (116.f * fZ - 16.f) / kappa;

  auto [ref_x, ref_y, ref_z] = REF_WHITE_D65;

  return Xyz(rX * ref_x, rY * ref_y, rZ * ref_z);
}


Lch_Ab Lab::to_lch_ab() const {
  const auto [l, c, h] = to_polar_color_space(m_values);

  return Lch_Ab(l, c, h);
};


void Lab::print() const {
  std::cout << "[Lab]" << "\nL: " << m_values[0] << "\na: " << m_values[1]
            << "\nb: " << m_values[2] << "\n\n";
}


float Lab::diff_cie_2000(const Lab &other) const {
  auto [L1, a1, b1] = m_values;
  auto [L2, a2, b2] = other.get_values();

  // parametric weighting factors set to unity
  const float k_L = 1.0f;
  const float k_C = 1.0f;
  const float k_H = 1.0f;

  // Sharma 2005, eqn. 2
  const float C1 = euclidean_norm(a1, b1);
  const float C2 = euclidean_norm(a2, b2);

  // Sharma 2005, eqn. 3
  const float C_mean = (C1 + C2) / 2.0f;

  // Sharma 2005, eqn. 4
  const float G_component_a = std::pow(25.0f, 7);
  const float G_component_b = std::pow(C_mean, 7);
  const float G =
      0.5f *
      (1.0f - std::sqrt(G_component_b / (G_component_b + G_component_a)));

  // Sharma 2005, eqn. 5
  const float a1_prime = a1 * (1 + G);
  const float a2_prime = a2 * (1 + G);

  // Sharma 2005, eqn. 6
  const float C1_prime = euclidean_norm(a1_prime, b1);
  const float C2_prime = euclidean_norm(a2_prime, b2);

  // Sharma 2005, eqn. 7
  const float h1_prime = to_degrees(std::atan2(b1, a1_prime));
  const float h2_prime = to_degrees(std::atan2(b2, a2_prime));

  // Sharma 2005, eqn. 8
  const float delta_L_prime = L2 - L1;

  // Sharma 2005, eqn. 9
  const float delta_C_prime = C2_prime - C1_prime;

  // Sharma 2005, eqn. 10 (modified by LLM)
  const float h1_corrected = (h1_prime >= 0.0f) ? h1_prime : h1_prime + 360.0f;
  const float h2_corrected = (h2_prime >= 0.0f) ? h2_prime : h2_prime + 360.0f;
  float delta_h_prime =
      std::fmod(h2_corrected - h1_corrected + 540.0f, 360.0f) - 180.0f;

  // Sharma 2005, eqn. 11
  const float delta_H_prime = 2.0f * std::sqrt(C1_prime * C2_prime) *
                              std::sin(to_radians(delta_h_prime / 2.0f));

  // Sharma 2005, eqn. 12
  const float L_prime_mean = (L1 + L2) / 2.0f;

  // Sharma 2005, eqn. 13
  const float C_prime_mean = (C1_prime + C2_prime) / 2.0f;

  // Sharma 2005, eqn. 14
  const float H_prime_mean = (std::abs(h1_corrected - h2_corrected) > 180.0f)
                                 ? (h1_corrected + h2_corrected + 360.0f) / 2.0f
                                 : (h1_corrected + h2_corrected) / 2.0f;

  // Sharma 2005, eqn. 15
  const float T = 1.0f - 0.17f * cos(to_radians(H_prime_mean - 30.0f)) +
                  0.24f * std::cos(to_radians(2.0f * H_prime_mean)) +
                  0.32f * std::cos(to_radians(3.0f * H_prime_mean + 6.0f)) -
                  0.20f * std::cos(to_radians(4.0f * H_prime_mean - 63.0f));

  // Sharma 2005, eqn. 16
  const float delta_theta =
      30.0f * std::exp(-1.0f * std::pow((H_prime_mean - 275.0f) / 25.0f, 2));

  // Sharma 2005, eqn. 17
  const float R_C_component = std::pow(C_prime_mean, 7);
  const float R_C =
      2.0f * std::sqrt(R_C_component / (R_C_component + G_component_a));

  // Sharma 2005, eqn. 18
  const float S_L_component = L_prime_mean - 50.0f;
  const float S_L =
      1.0f + (0.015f * S_L_component * S_L_component) /
                 std::sqrt(20.0f + std::pow(L_prime_mean - 50.0f, 2));

  // Sharma 2005, eqn. 19
  const float S_C = 1.0f + 0.045f * C_prime_mean;

  // Sharma 2005, eqn. 20
  const float S_H = 1.0f + 0.015f * C_prime_mean * T;

  // Sharma 2005, eqn. 21
  const float R_T = -R_C * std::sin(to_radians(2.0f * delta_theta));

  // Sharma 2005, eqn. 22
  const float L_term = delta_L_prime / (k_L * S_L);
  const float C_term = delta_C_prime / (k_C * S_C);
  const float H_term = delta_H_prime / (k_H * S_H);
  const float delta_E = std::sqrt(L_term * L_term + C_term * C_term +
                                  H_term * H_term + R_T * C_term * H_term);

  return delta_E;
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
    if (std::min(255.f, std::max(0.f, c)) != c) {
      throw std::domain_error("Channel initalized outside of range [0, 255].");
    }
  };

  validate(r);
  validate(g);
  validate(b);
};


Xyz Rgb::to_xyz() const {

  static const Matrix M_matrix =
      create_to_xyz_transformation_matrix(REF_WHITE_D65);

  auto [r, g, b] = m_values;

  // Step 1: Normalize input RGB [0–255] -> [0.0–1.0]
  const float r_lin = remove_gamma(r / 255.f);
  const float g_lin = remove_gamma(g / 255.f);
  const float b_lin = remove_gamma(b / 255.f);

  // Step 2: Convert to XYZ using matrix
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


// std::array<float, 3>
// multiplyMatrixInline(const std::array<std::array<float, 3>, 3> &m,
//                      const std::array<float, 3> &v) {
//   return {
//       m[0][0] * v[0] + m[0][1] * v[1] + m[0][2] * v[2],
//       m[1][0] * v[0] + m[1][1] * v[1] + m[1][2] * v[2],
//       m[2][0] * v[0] + m[2][1] * v[1] + m[2][2] * v[2],
//   };
// }

// std::array<std::array<float, 3>, 3>
// create_to_xyz_transformation_matrix_v2(std::array<float, 3> ref_white) {
//   // Chromaticities of primaries
//   const float r_x = 0.6400f, r_y = 0.3300f;
//   const float g_x = 0.3000f, g_y = 0.6000f;
//   const float b_x = 0.1500f, b_y = 0.0600f;

//   // Compute XYZ coordinates of each primary
//   const float r_X = r_x / r_y;
//   const float r_Y = 1.f;
//   const float r_Z = (1 - r_x - r_y) / r_y;

//   const float g_X = g_x / g_y;
//   const float g_Y = 1.f;
//   const float g_Z = (1 - g_x - g_y) / g_y;

//   const float b_X = b_x / b_y;
//   const float b_Y = 1.f;
//   const float b_Z = (1 - b_x - b_y) / b_y;

//   // Build RGB-to-XYZ matrix from primaries
//   Matrix M({
//       {r_X, g_X, b_X},
//       {r_Y, g_Y, b_Y},
//       {r_Z, g_Z, b_Z},
//   });

//   // Invert and scale to match reference white
//   Matrix S = M.invert().multiply(arr_to_column(ref_white));
//   Matrix scaled = M.column_wise_scaling(S);

//   // Convert back to std::array
//   std::array<std::array<float, 3>, 3> out{};
//   for (size_t i = 0; i < 3; i++)
//     for (size_t j = 0; j < 3; j++)
//       out[i][j] = scaled(i, j);

//   return out;
// }

// Rgb Xyz::to_rgb() const {
//   Timer timer("to_rgb");
//   static const std::array<std::array<float, 3>, 3> xyz_transformation_matrix
//   =
//       create_to_xyz_transformation_matrix_v2(REF_WHITE_D65);

//   std::array<float, 3> linRGB = multiplyMatrixInline(
//       xyz_transformation_matrix, {m_values[0], m_values[1], m_values[2]});

//   const float r_corr = apply_gamma(linRGB[0]);
//   const float g_corr = apply_gamma(linRGB[1]);
//   const float b_corr = apply_gamma(linRGB[2]);

//   const float r_norm = std::clamp(r_corr, 0.f, 1.f) * 255.f;
//   const float g_norm = std::clamp(g_corr, 0.f, 1.f) * 255.f;
//   const float b_norm = std::clamp(b_corr, 0.f, 1.f) * 255.f;

//   return Rgb(r_norm, g_norm, b_norm);
// }


Rgb Xyz::to_rgb() const {
  Timer timer("to_rgb");

  const auto [x, y, z] = m_values;

  // matrix multiplication using XYZ-to-sRGB transformation matrix
  const float r_lin = 3.2404542f * x - 1.5371385f * y - 0.4985314f * z;
  const float g_lin = -0.9692660f * x + 1.8760108f * y + 0.0415560f * z;
  const float b_lin = 0.0556434f * x - 0.2040259f * y + 1.0572252f * z;

  const float r_norm = std::clamp(apply_gamma(r_lin) * 255.f, 0.f, 255.f);
  const float g_norm = std::clamp(apply_gamma(g_lin) * 255.f, 0.f, 255.f);
  const float b_norm = std::clamp(apply_gamma(b_lin) * 255.f, 0.f, 255.f);

  return Rgb(r_norm, g_norm, b_norm);
}


Lab Xyz::to_lab() const {
  auto [w_X, w_Y, w_Z] = REF_WHITE_D65;

  const float xR = m_values[0] / w_X;
  const float yR = m_values[1] / w_Y;
  const float zR = m_values[2] / w_Z;

  const float fX = (xR > epsilon) ? std::cbrt(xR) : (kappa * xR + 16) / 116.f;
  const float fY = (yR > epsilon) ? std::cbrt(yR) : (kappa * yR + 16) / 116.f;
  const float fZ = (zR > epsilon) ? std::cbrt(zR) : (kappa * zR + 16) / 116.f;

  const float l = 116.f * fY - 16;
  const float a = 500.f * (fX - fY);
  const float b = 200.f * (fY - fZ);

  return Lab(l, a, b);
}


void Xyz::print() const {
  std::cout << "[XYZ]" << "\nX: " << m_values[0] << "\nY: " << m_values[1]
            << "\nZ: " << m_values[2] << "\n\n";
}

} // namespace clrspc
