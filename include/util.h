#pragma once

#include "../include/Color_Space.h"
#include "../include/picture.h"

#include <array>
#include <thread>
#include <type_traits>
#include <vector>

constexpr int BLOCK_SIZE = 16;

int distSquared(const clrspc::Rgb &colorA, const clrspc::Rgb &colorB);

std::array<double, 3>
multiplyMatrix(const std::array<std::array<double, 3>, 3> &matrix,
               const std::array<double, 3> &vector);

clrspc::Lab getAverage(const Bitmap &bitmap, int originX, int originY);

std::vector<std::vector<int>>
buildLookupTable(const Bitmap &bitmap,
                 const std::vector<clrspc::Lab> &quantColors);

size_t findClosestColorIdx(const clrspc::Lab &targetColor,
                           const std::vector<clrspc::Lab> &quantColors);


template <typename T> auto euclidean_norm(const T xMag, const T yMag) {
  return std::sqrt(xMag * xMag + yMag * yMag);
}


template <typename Func>
void process2dInParallel(int height, int width, Func func) {

  static const int NUM_THREADS =
      std::min(static_cast<int>(std::thread::hardware_concurrency()), height);
  static const int CHUNK_SIZE = height / NUM_THREADS;

  std::vector<std::thread> threads;

  for (int t = 0; t < NUM_THREADS; ++t) {
    threads.emplace_back([=]() {
      int startRow = t * CHUNK_SIZE;
      int endRow = (t == NUM_THREADS - 1) ? height : (t + 1) * CHUNK_SIZE;

      for (int j = startRow; j < endRow; ++j) {
        for (int i = 0; i < width; ++i) {
          func(i, j);
        }
      }
    });
  }

  for (auto &thread : threads) {
    thread.join();
  }
}
