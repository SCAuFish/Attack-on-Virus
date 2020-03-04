#pragma once
#include "Light.h"
#include "PointCloud.h"

class PointLight : public Light
{
public:
	glm::mat4 model; // shared with representation
	GLuint positionLoc, virtualObjectLoc;
	PointCloud* representation;

	PointLight(float xPos, float yPos, float zPos, float r = 3.f, float g = 3.f, float b = 3.f);
	~PointLight();

	void setShaderProgram(GLuint programInd);
	void sendToShader();
	void showRepresentation(GLuint modelLoc, GLuint colorLoc);
};

