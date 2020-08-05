#pragma once
#ifndef _CLOUD_H_
#define _CLOUD_H_

#include <random>
#include <stdlib.h>

/**
 * Refer to https://www.gamedev.net/articles/programming/general-and-gameplay-programming/simple-clouds-part-1-r2085/
 * for more information
 * author: Cheng Shen; Manxue Li
 */
const unsigned int seed = 100;
const int noiseMapSize = 32;
const int cloudMapSize = 256;

class Cloud
{
private:
    // The higher cover is, the more "black part" there will be
    float cover = 20.0f;
    // The edge between cloud and the sky
    float sharpness = 0.95f;

public:
    Cloud();
    ~Cloud();
    float noiseMap[noiseMapSize * noiseMapSize];
    float cloudMap[cloudMapSize * cloudMapSize];

    float Noise(int x, int y);  // return a value between -1 and 1
    void  setNoise();           // use Noise function to set noiseMap
    float interpolate(float x, float y, float* map);    // interpolate when mapping noisemap to larger array
    void  overlapOctaves();
    void  ExpFilter();
};

#endif // !_CLOUD_H_
