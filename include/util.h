#pragma once

#include "../include/Color_Space.h"
#include "../include/picture.h"

#include <array>
#include <thread>
#include <type_traits>
#include <vector>

constexpr int BLOCK_SIZE = 16;
constexpr float ONE_SIXTEENTH = 0.0625f;

float distSquared(clrspc::Lab const& colorA, clrspc::Lab const& colorB);

std::array<float, 3> multiplyMatrix(
    std::array<std::array<float, 3>, 3> const& matrix, std::array<float, 3> const& vector);

clrspc::Lab getAverage(Bitmap const& bitmap, int originX, int originY);

std::vector<std::vector<int>> buildLookupTable(
    Bitmap const& bitmap, std::vector<clrspc::Lab> const& quantColors);

size_t findClosestColorIdx(
    clrspc::Lab const& targetColor, std::vector<clrspc::Lab> const& quantColors);

template<typename T> auto euclidean_norm(T const xMag, T const yMag)
{
    return std::sqrt(xMag * xMag + yMag * yMag);
}

template<typename Func> void process2dInParallel(int height, int width, Func func)
{

    static int const NUM_THREADS
        = std::min(static_cast<int>(std::thread::hardware_concurrency()), height);
    static int const CHUNK_SIZE = height / NUM_THREADS;

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

    for (auto& thread : threads) {
        thread.join();
    }
}
