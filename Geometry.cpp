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

void Geometry::loadObjFile(std::string filename, int mode)
{
	this->object = new PointCloud(filename, 1, mode);
	this->kdTree = new SpaceTree(this->object->points);

	// show first-level bounding box and filename
	std::cout << filename << std::endl;
	printf("(%f, %f, %f) - (%f, %f, %f)\n", this->kdTree->root->x_min, this->kdTree->root->y_min, this->kdTree->root->z_min,
		this->kdTree->root->x_max, this->kdTree->root->y_max, this->kdTree->root->z_max);
}

void Geometry::draw(glm::mat4 C)
{
	this->sendModelToShader(C * (this->object->model));
	this->object->draw();
}

void Geometry::changeModel(glm::mat4 C)
{
	this->object->model = C * (this->object->model);
}

void Geometry::setModelLoc(GLuint modelLoc) {
	this->modelLoc = modelLoc;
}

void Geometry::update()
{
}
