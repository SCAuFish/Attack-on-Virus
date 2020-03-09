#include "Cloud.h"

Cloud::Cloud()
{
    this->setNoise();
}

Cloud::~Cloud()
{
}

float Cloud::Noise(int x, int y)
{
    // Return value is between -1 and 1 (hopefully?)
    int n = x + y * 57 + seed * 131;
    n = (n << 13) ^ n;
    return (1.0f - ((n * (n * n * 15731 + 789221) +
        1376312589) & 0x7fffffff) * 0.000000000931322574615478515625f);
}

void Cloud::setNoise()
{
    float temp[noiseMapSize + 2][noiseMapSize + 2];
    for (int i = 1; i <= noiseMapSize; i++) {
        for (int j = 1; j <= noiseMapSize; j++) {
            temp[i][j] = 128.0f + Noise(i, j) * 128.0f;
        }
    }

    // Mirror the texture at edges
    for (int i = 1; i <= noiseMapSize; i++) {
        temp[0][i] = temp[noiseMapSize][i];
        temp[noiseMapSize + 1][i] = temp[1][i];
        temp[i][0] = temp[i][noiseMapSize];
        temp[i][noiseMapSize + 1] = temp[i][1];
    }
    temp[0][0] = temp[noiseMapSize][noiseMapSize];
    temp[noiseMapSize + 1][noiseMapSize + 1] = temp[1][1];
    temp[0][noiseMapSize + 1] = temp[noiseMapSize][1];
    temp[noiseMapSize + 1][0] = temp[1][noiseMapSize];

    // Smoothing and set value for noise map
    for (int i = 1; i <= noiseMapSize; i++) {
        for (int j = 1; j <= noiseMapSize; j++) {
            // Ignore padding spaces
            float center = temp[i][j] / 4.f;
            float sides = (temp[i + 1][j] + temp[i][j + 1] + temp[i - 1][j] + temp[i][j - 1]) / 8.f;
            float corner = (temp[i - 1][j - 1] + temp[i + 1][j - 1] + temp[i - 1][j + 1] + temp[i + 1][j + 1]) / 16.f;

            noiseMap[(i-1) * noiseMapSize + (j - 1)] = center + sides + corner;
        }
    }
}

float Cloud::interpolate(float x, float y, float* map)
{
    // float x and y represents the floating-coordinates
    int x0 = (int)x;
    int y0 = (int)y;
    int x1 = (x0 + 1) % noiseMapSize;
    int y1 = (y0 + 1) % noiseMapSize;

    float xFrac = x - x0;
    float yFrac = y - y0;

    float bot = map[x0 * noiseMapSize + y0] + xFrac * (map[x1 * noiseMapSize + y0] - map[x0 * noiseMapSize + y0]);
    float top = map[x0 * noiseMapSize + y1] + xFrac * (map[x1 * noiseMapSize + y1] - map[x0 * noiseMapSize + y1]);

    return (bot + yFrac * (top - bot));
}

void Cloud::overlapOctaves()
{
    // Create different scaling level noise maps and pile them up to make cloud map
    for (int i = 0; i < cloudMapSize; i++) {
        cloudMap[i] = 0;
    }

    // Create 4 octaves
    for (int octave = 0; octave < 4; octave++)
        for (int x = 0; x < cloudMapSize; x++)
            for (int y = 0; y < cloudMapSize; y++)
            {
                float scale = 1 / pow(2, 3 - octave);
                float noise = interpolate(x * scale, y * scale, noiseMap);
                cloudMap[(y * cloudMapSize) + x] += noise / pow(2, octave);
            }
}

void Cloud::ExpFilter()
{
    for (int x = 0; x < cloudMapSize * cloudMapSize; x++)
    {
        float c = cloudMap[x] - (255.0f - cover);
        if (c < 0)     c = 0;
        cloudMap[x] = 255.0f - ((float)(pow(sharpness, c)) * 255.0f);
    }
}
