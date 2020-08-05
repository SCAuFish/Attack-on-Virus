#pragma once
#ifndef _ROBOT_H_
#define _ROBOT_H_

#include "Node.h"
#include "Geometry.h"
#include "Transform.h"

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

extern int period, updateCount;

Transform* buildRobotArmy();

Transform* buildRobot();

glm::mat4 leftArmSwing(int currentCount, glm::mat4& model);

glm::mat4 rightArmSwing(int currentCount, glm::mat4& model);

glm::mat4 leftLegSwing(int currentCount, glm::mat4& model);

glm::mat4 rightLegSwing(int currentCount, glm::mat4& model);

glm::mat4 headShake(int currentCount, glm::mat4& model);
#endif