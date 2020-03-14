#include "SpaceTreeNode.h"

SpaceTreeNode::SpaceTreeNode(glm::vec3 v)
{
	vertex = v;
	x_min = x_max = v[0];
	y_min = y_max = v[1];
	z_min = z_max = v[2];
	left = right = nullptr;
}

SpaceTreeNode::~SpaceTreeNode()
{
	delete left;
	delete right;
}
