#include "PointLight.h"

PointLight::PointLight(float xPos, float yPos, float zPos, float r, float g, float b) {
	representation = new PointCloud(".\\Objects\\sphere.obj", 1, xPos, yPos, zPos);
	representation->scale(0.2f, 0.2f, 0.2f);
	color = glm::mat3(1.f);
	color[0][0] = r;
	color[1][1] = g;
	color[2][2] = b;

	// Location in the world
	model = representation->getModel();
}

PointLight::~PointLight() {
	delete representation;
}

void PointLight::setShaderProgram(GLuint programInd) {
	program = programInd;

	lightColorLoc = glGetUniformLocation(program, "pointLightColor");
	positionLoc = glGetUniformLocation(program, "lightPosition");
	virtualObjectLoc = glGetUniformLocation(program, "virtualObject");
}

void PointLight::sendToShader() {
	glUseProgram(program);
	glUniformMatrix3fv(lightColorLoc, 1, GL_FALSE, glm::value_ptr(color));
	model = representation->getModel();
	glm::vec3 position = model * glm::vec4(0, 0, 0, 1);
	glUniform3fv(positionLoc, 1, glm::value_ptr(position));
}

void PointLight::showRepresentation(GLuint modelLoc, GLuint colorLoc) {
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(representation->getModel()));
	glUniform3fv(colorLoc, 1, glm::value_ptr(color * glm::vec3(1.f, 1.f, 1.f)));
	glUniform1i(virtualObjectLoc, GLuint(1));
	representation->draw();
	glUniform1i(virtualObjectLoc, GLuint(0));
}