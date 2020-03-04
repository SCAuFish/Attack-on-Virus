#include "DirectionalLight.h"

DirectionalLight::DirectionalLight(GLuint programInd, float r, float g, float b):
	lightDirLoc(0)
{
	program = programInd;
	lightColorLoc = 0;
	color = glm::mat3(1.0f);
	color[0][0] = r;
	color[1][1] = g;
	color[2][2] = b;
	// Default pointing to x-axis direction
	light2world = glm::mat4(1.0f);
	direction = glm::vec3(1.0f, -1.0f, 1.0f);
}

DirectionalLight::~DirectionalLight() {}

void DirectionalLight::setShaderProgram(GLuint programInd) {
	program = programInd;

	lightColorLoc = glGetUniformLocation(program, "dirLightColor");
	lightDirLoc = glGetUniformLocation(program, "lightDir");
}

void DirectionalLight::sendToShader() {
	glUseProgram(program);
	glUniformMatrix3fv(lightColorLoc, 1, GL_FALSE, glm::value_ptr(color));
	glUniform3fv(lightDirLoc, 1, glm::value_ptr(direction));
}

void DirectionalLight::update()
{
	direction = glm::rotate(0.05f, glm::vec3(.0f, 1.0f, .0f)) * glm::vec4(direction, 1.0f);
}
