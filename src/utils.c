#include "utils.h"
#include "raylib.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

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
    Vector2 dpi = {1.0f, 1.0f}; // GetWindowScaleDPI();

    // Adjust the requested size by the DPI factor
    int finalWidth = (int)(width * dpi.x);
    int finalHeight = (int)(height * dpi.y);

    SetWindowSize(finalWidth, finalHeight);
}

const char *FormatNumber(unsigned long long n)
{
    static char buffer[32];
    char temp[32];
    int res = sprintf(temp, "%llu", n);

    int count = 0;
    int dest = 0;

    for (int i = res - 1; i >= 0; i--)
    {
        if (count > 0 && count % 3 == 0)
        {
            buffer[dest++] = '.';
        }
        buffer[dest++] = temp[i];
        count++;
    }
    buffer[dest] = '\0';

    // Reverse the string back
    for (int i = 0; i < dest / 2; i++)
    {
        char t = buffer[i];
        buffer[i] = buffer[dest - i - 1];
        buffer[dest - i - 1] = t;
    }

    return buffer;
}

const char *TextFitting(const char *src, unsigned int maxWidth, Font *font, float fontSize, float spacing)
{
    Font currentFont = (font == NULL) ? GetFontDefault() : *font;

    char *result = (char *)malloc(strlen(src) * 1.2f + 1);
    result[0] = '\0';

    char *buffer = strdup(src);
    char *word = strtok(buffer, " ");
    float currentLineWidth = 0;
    float spaceWidth = MeasureTextEx(currentFont, " ", fontSize, spacing).x;

    while (word != NULL)
    {
        Vector2 wordSize = MeasureTextEx(currentFont, word, fontSize, spacing);

        // Check if we need a newline
        if (currentLineWidth + wordSize.x > maxWidth)
        {
            strcat(result, "\n");
            currentLineWidth = 0;
        }
        else if (currentLineWidth > 0)
        {
            strcat(result, " ");
            currentLineWidth += spaceWidth;
        }

        strcat(result, word);
        currentLineWidth += wordSize.x;
        word = strtok(NULL, " ");
    }

    free(buffer);
    return result; // Caller is responsible for freeing this memory!
}
