#include "utils.h"
#include "raylib.h"
#include <stdlib.h>

unsigned int SampleFromWeightedDistribution(const unsigned short *weights, unsigned int count)
{
    unsigned int totalWeight = 0;
    for (unsigned int i = 0; i < count; i++)
    {
        totalWeight += weights[i];
    }

    unsigned int r = GetRandomValue(0, totalWeight - 1);
    unsigned int cumulativeWeight = 0;
    for (unsigned int i = 0; i < count; i++)
    {
        cumulativeWeight += weights[i];
        if (r < cumulativeWeight)
        {
            return i;
        }
    }

    // This should never happen if the weights are valid
    TraceLog(LOG_ERROR, "SampleFromWeightedDistribution: Invalid weights");
    exit(1);
}

void SafeSetWindowSize(int width, int height)
{
    // Get the scaling factor (e.g., 1.5 for 150% scaling)
    Vector2 dpi = GetWindowScaleDPI();

    // Adjust the requested size by the DPI factor
    int finalWidth = (int)(width * dpi.x);
    int finalHeight = (int)(height * dpi.y);

    SetWindowSize(finalWidth, finalHeight);
}