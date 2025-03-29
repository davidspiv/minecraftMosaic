#ifndef UTIL_H
#define UTIL_H

struct Color {
  int r;
  int g;
  int b;
};

// convert sRGB channel to a linear value
double linearizeRGB(int channel);

int delinearizeRGB(double linearRGB);

#endif
