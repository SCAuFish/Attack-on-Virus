#pragma once
#ifndef _DIRECTIONALLIGHT_H_
#define _DIRECTIONALLIGHT_H_

#include "Light.h"
#include <iostream>

class DirectionalLight : public Light
{
public:
	GLuint lightDirLoc;

	// Direction could be set constant as pointing at x-axis
	// in light model; modify light2world matrix to realize rotation
	glm::mat4 light2world;
	glm::vec3 direction;

	void setShaderProgram(GLuint programInd);
	void sendToShader();
	void update();

	DirectionalLight(GLuint programInd, float r, float, float b);
	~DirectionalLight();
};

#endif

