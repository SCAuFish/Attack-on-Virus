#pragma once
#ifndef _NODE_H_
#define _NODE_H_

#ifdef __APPLE__
#define GLFW_INCLUDE_GLCOREARB
#include <OpenGL/gl3.h>
#else
#include <GL/glew.h>
#endif
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Node
{
public:
	virtual void draw(glm::mat4 trans) = 0;
	virtual void changeModel(glm::mat4 trans) = 0;
	virtual void update() = 0;
	virtual void setModelLoc(GLuint modelLoc) = 0;
};
#endif