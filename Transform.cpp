#include "Transform.h"
#include <iostream>
Transform::Transform() : Node()
{
	this->hasUpdateFunc = false;
	this->M = glm::mat4(1.0f);
	this->prevAccumulatedM = glm::mat4(1.0f);
}

Transform::~Transform()
{
	for (Node* child: children) {
		delete child;
	}

	delete this;
}

void Transform::addChild(Node* child)
{
	this->children.push_back(child);
}

bool Transform::removeChild(Node* child)
{
	auto iter = this->children.begin();
	while (iter != this->children.end()) {
		if (*iter == child) {
			this->children.erase(iter);
			return true;
		}

		iter++;
	}
	return false;
}

void Transform::changeModel(glm::mat4 C) {
	for (Node* child : this->children) {
		child->changeModel(C);
	}
}

void Transform::draw(glm::mat4 C)
{
	glm::mat4 accumulated = C * (this->M);
	this->prevAccumulatedM = accumulated;

	glBindTexture(GL_TEXTURE_2D, this->textureId);
	for (Node* child : this->children) {
		child->draw(accumulated);
	}
}

void Transform::setModelLoc(GLuint modelLoc)
{
	for (Node* child : this->children) {
		child->setModelLoc(modelLoc);
	}
}

void Transform::update()
{
	// TODO: update itself
	if (this->hasUpdateFunc) {
		(this->updateFunc)(1, this->M);
	}

	for (Node* child : this->children) {
		if (child) child->update();
	}
}

void Transform::setUpdateFunc(glm::mat4(* func_ptr)(int, glm::mat4&))
{
	this->hasUpdateFunc = true;
	this->updateFunc = func_ptr;
}

void Transform::setTextureId(unsigned int textureId)
{
	this->textureId = textureId;
}
