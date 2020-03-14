#pragma once
#ifndef _SPACE_TREE_H_
#define _SPACE_TREE_H_

#include "SpaceTreeNode.h"
#include <algorithm>
// This is a KD-Tree that is used to label bounding boxes of different objects
class SpaceTree
{
private:
	SpaceTreeNode* buildKDTree(std::vector<glm::vec3>& v, int dim, int left, int right);
	bool bbIntersect(SpaceTreeNode* lhs, SpaceTreeNode* rhs, int dim, const glm::mat4& lhsModel, const glm::mat4& rhsModel);
	// bool bbIntersect(SpaceTreeNode* lhs, SpaceTreeNode* rhs, int dim);
	bool intersectHelper(SpaceTreeNode* lhs, SpaceTreeNode* rhs, int dim, int levelLimit, const glm::mat4& lhsModel, const glm::mat4& rhsModel);
	// bool intersectHelper(SpaceTreeNode* lhs, SpaceTreeNode* rhs, int dim, int levelLimit);

public:
	//Instance variables
	SpaceTreeNode* root;

	//functions
	SpaceTree(std::vector<glm::vec3> vs);
	~SpaceTree();

	bool intersectWith(SpaceTree* other, const glm::mat4& model, const glm::mat4& otherModel);

	// bool intersectWith(SpaceTree* other);
};

#endif