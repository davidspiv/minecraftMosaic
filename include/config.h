#pragma once

#include <cstddef> // for size_t

// Texture filtering threshold.
// Higher values allow more variation within a texture tile.
// Lower values enforce stricter uniformity (less noisy textures).
// DELETE texture.dat file after changing this setting
constexpr float DIFF_THRESHOLD = 35.5f; // about [1-100]

// Gaussian blur radius for preprocessing
// Higher for more blur
constexpr int GAUSSIAN_BLUR_RADIUS = 15; // about [3-100]
