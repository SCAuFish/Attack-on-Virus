#pragma once
#include "Cube.h"
class Skybox :
	public Cube
{
public:
	unsigned int cubemapTexture;
	Skybox(float size);
	void draw();
	void setTextureID(unsigned int textureID);
};

