#pragma once
#include <vector>;

unsigned char* loadPPM(const char* filename, int& width, int& height);
unsigned int loadTexture(std::vector<char*> filenames);
unsigned int load2DTexture(char* filenames);