#pragma once
#ifndef _LIGHT_H_
#define _LIGHT_H_

#ifdef __APPLE__
#include <OpenGL/gl3.h>
#else
#include <GL/glew.h>
#endif
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/transform.hpp>

class Light
{
	// TODO: Base Class for different light types
public:
	// Use a matrix so that a color can multiply with material color
// Expecting a diagonal matrix
	glm::mat3 color;
	GLuint program;
	GLuint lightColorLoc;

	virtual void setShaderProgram(GLuint programInd) = 0;
	virtual void sendToShader() = 0;
	virtual void update() {};
};


#endif // !_LIGHT_H_
