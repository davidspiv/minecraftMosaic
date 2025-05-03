#pragma once

#include <array>
#include <cstddef>
#include <iomanip>
#include <stdexcept>
#include <vector>

class Matrix {
public:
  Matrix(int rows, int cols);
  explicit Matrix(const std::vector<std::vector<float>> &arr_2d);

  inline size_t rows() const { return m_rows; }
  inline size_t cols() const { return m_cols; }

  const float &operator()(int i, int j) const; // Read
  float &operator()(int i, int j);             // Assign

  Matrix multiply(const Matrix &other) const;
  Matrix column_wise_scaling(const Matrix &other) const;
  Matrix invert() const;
  std::vector<float> flatten() const;
  Matrix transpose() const;

private:
  std::vector<std::vector<float>> m_values;
  size_t m_rows;
  size_t m_cols;
};

std::ostream &operator<<(std::ostream &os, const Matrix &a);

Matrix arr_to_column(std::array<float, 3> arr);


Matrix create_to_xyz_transformation_matrix(std::array<float, 3> ref_white);
