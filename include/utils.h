#ifndef UTILS_H
#define UTILS_H

#include "raylib.h"

#define min(a, b) (a > b ? b : a)
#define max(a, b) (a > b ? a : b)
#define clamp(x, lower, upper) (max(lower, min(x, upper)))

unsigned int SampleFromWeightedDistribution(const unsigned short *weights, unsigned int count);

void SafeSetWindowSize(int width, int height);

const char *FormatNumber(unsigned long long n);

const char *TextFitting(const char *src, unsigned int maxWidth, Font *font, float fontSize, float spacing);

#endif