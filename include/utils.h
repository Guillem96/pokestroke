#ifndef UTILS_H
#define UTILS_H

#define min(a, b) (a > b ? b : a)
#define max(a, b) (a > b ? a : b)
#define clamp(x, lower, upper) (max(lower, min(x, upper)))

unsigned int SampleFromWeightedDistribution(const unsigned short *weights, unsigned int count);

void SafeSetWindowSize(int width, int height);

#endif