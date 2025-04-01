#ifndef MISC_PICS_H
#define MISC_PICS_H

#include "../include/color.h"
#include "../include/picture.h"

#include <vector>

Bitmap createQuantizedPic(const Bitmap &bitmap);

void createAtlasPic(const std::vector<Bitmap> &validTextures);

const std::vector<StdRGB> endesgaPalette = {
    {190, 74, 47},   {215, 118, 67},  {234, 212, 170}, {228, 166, 114},
    {184, 111, 80},  {115, 62, 57},   {62, 39, 49},    {162, 38, 51},
    {228, 59, 68},   {247, 118, 34},  {254, 174, 52},  {254, 231, 97},
    {99, 199, 77},   {62, 137, 72},   {38, 92, 66},    {25, 60, 62},
    {18, 78, 137},   {0, 153, 219},   {44, 232, 245},  {255, 255, 255},
    {192, 203, 220}, {139, 155, 180}, {90, 105, 136},  {58, 68, 102},
    {38, 43, 68},    {24, 20, 37},    {255, 0, 68},    {104, 56, 108},
    {181, 80, 136},  {246, 117, 122}, {232, 183, 150}, {194, 133, 105}};

const std::vector<StdRGB> apolloPalette = {
    {23, 32, 56},    {37, 58, 94},    {60, 94, 139},   {79, 143, 186},
    {115, 190, 211}, {164, 221, 219}, {25, 51, 45},    {37, 86, 46},
    {70, 130, 50},   {117, 167, 67},  {168, 202, 88},  {208, 218, 145},
    {77, 43, 50},    {122, 72, 65},   {173, 119, 87},  {192, 148, 115},
    {215, 181, 148}, {231, 213, 179}, {52, 28, 39},    {96, 44, 44},
    {136, 75, 43},   {190, 119, 43},  {222, 158, 65},  {232, 193, 112},
    {36, 21, 39},    {65, 29, 49},    {117, 36, 56},   {163, 48, 48},
    {207, 87, 60},   {218, 134, 62},  {30, 29, 57},    {64, 39, 81},
    {122, 54, 123},  {162, 62, 140},  {198, 81, 151},  {223, 132, 165},
    {9, 10, 20},     {16, 20, 31},    {21, 29, 40},    {32, 46, 55},
    {57, 74, 80},    {87, 114, 119},  {129, 151, 150}, {168, 181, 178},
    {199, 207, 204}, {235, 237, 233}};


const std::vector<StdRGB> resurrectPalette = {
    {46, 34, 47},    {62, 53, 70},    {98, 85, 101},   {150, 108, 108},
    {171, 148, 122}, {105, 79, 98},   {127, 112, 138}, {155, 171, 178},
    {199, 220, 208}, {255, 255, 255}, {110, 39, 39},   {179, 56, 49},
    {234, 79, 54},   {245, 125, 74},  {174, 35, 52},   {232, 59, 59},
    {251, 107, 29},  {247, 150, 23},  {249, 194, 43},  {122, 48, 69},
    {158, 69, 57},   {205, 104, 61},  {230, 144, 78},  {251, 185, 84},
    {76, 62, 36},    {103, 102, 51},  {162, 169, 71},  {213, 224, 75},
    {251, 255, 134}, {22, 90, 76},    {35, 144, 99},   {30, 188, 115},
    {145, 219, 105}, {205, 223, 108}, {49, 54, 56},    {55, 78, 74},
    {84, 126, 100},  {146, 169, 132}, {178, 186, 144}, {11, 94, 101},
    {11, 138, 143},  {14, 175, 155},  {48, 225, 185},  {143, 248, 226},
    {50, 51, 83},    {72, 74, 119},   {77, 101, 180},  {77, 155, 230},
    {143, 211, 255}, {69, 41, 63},    {107, 62, 117},  {144, 94, 169},
    {168, 132, 243}, {234, 173, 237}, {117, 60, 84},   {162, 75, 111},
    {207, 101, 127}, {237, 128, 153}, {131, 28, 93},   {195, 36, 84},
    {240, 79, 120},  {246, 129, 129}, {252, 167, 144}, {253, 203, 176}};


const std::vector<StdRGB> zughyPalette = {
    {71, 45, 60},    {94, 54, 67},    {122, 68, 74},   {160, 91, 83},
    {191, 121, 88},  {238, 161, 96},  {244, 204, 161}, {182, 213, 60},
    {113, 170, 52},  {57, 123, 68},   {60, 89, 86},    {48, 44, 46},
    {90, 83, 83},    {125, 112, 113}, {160, 147, 142}, {207, 198, 184},
    {223, 246, 245}, {138, 235, 241}, {40, 204, 223},  {57, 120, 168},
    {57, 71, 120},   {57, 49, 75},    {86, 64, 100},   {142, 71, 140},
    {205, 96, 147},  {255, 174, 182}, {244, 180, 27},  {244, 126, 27}};

#endif
