#pragma once
#ifndef _SPACE_TREE_H_
#define _SPACE_TREE_H_

#include "SpaceTreeNode.h"
#include <algorithm>
// This is a KD-Tree that is used to label bounding boxes of different objects

extern bool debugOn;

class SpaceTree
{
private:
	SpaceTreeNode* buildKDTree(const std::vector<glm::vec3>& points, std::vector<std::vector<unsigned int>>& triangles, 
		int dim, int left, int right);
	bool bbIntersect(SpaceTreeNode* lhs, SpaceTreeNode* rhs, const glm::mat4& lhsModel, const glm::mat4& rhsModel);
	bool intersectHelper(SpaceTreeNode* lhs, SpaceTreeNode* rhs, const glm::mat4& lhsModel, const glm::mat4& rhsModel, std::vector<unsigned int>& lhs_triangles, std::vector<unsigned int>& rhs_triangles);
	bool triangleIntersect(SpaceTreeNode* lhs, SpaceTreeNode* rhs, const glm::mat4& lhsModel, const glm::mat4& rhsModel, std::vector<unsigned int>& lhs_triangles, std::vector<unsigned int>& rhs_triangles);

public:
	//Instance variables
	SpaceTreeNode* root;
	const std::vector<glm::vec3>& points;

	//functions
	SpaceTree(const std::vector<glm::vec3>& points, std::vector<unsigned int> triangles);
	~SpaceTree();

	bool intersectWith(SpaceTree* other, const glm::mat4& model, const glm::mat4& otherModel, 
		std::vector<unsigned int>& lhs_triangles, std::vector<unsigned int>& rhs_triangles);
};

#endif