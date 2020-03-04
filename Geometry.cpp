#include "Geometry.h"
#include <iostream>

void Geometry::sendModelToShader(glm::mat4 model)
{
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
}

Geometry::Geometry()
{
	// do nothing
}

Geometry::~Geometry()
{
	delete object;
}

void Geometry::loadObjFile(std::string filename)
{
	this->object = new PointCloud(filename, 1, 1);
}

void Geometry::draw(glm::mat4 C)
{
	this->sendModelToShader(C * (this->object->model));
	this->object->draw();
}

void Geometry::setModelLoc(GLuint modelLoc) {
	this->modelLoc = modelLoc;
}

void Geometry::update()
{
}
