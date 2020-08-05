#pragma once
#ifndef _SPACETREENODE_H_
#define _SPACETREENODE_H_
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
#include <gl/glu.h> 
#include <iostream>
#include <vector>
#include <string>

// Node that is used in SpaceTree
class SpaceTreeNode
{
public:
	// Instance variables
	const std::vector<glm::vec3>& points;
	std::vector<unsigned int> triangle;    // store triangle in leaf node
	float x_min, x_max, y_min, y_max, z_min, z_max;
	SpaceTreeNode* left, * right;

	SpaceTreeNode(const std::vector<glm::vec3>& points);
	~SpaceTreeNode();
};

#endif