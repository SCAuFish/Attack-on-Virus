#include "SpaceTreeNode.h"
#include <limits>

SpaceTreeNode::SpaceTreeNode(const std::vector<glm::vec3>& points) : points(points)
{
	x_min = y_min = z_min = std::numeric_limits<float>::max();
	x_max = y_max = z_max = std::numeric_limits<float>::min();
	left = right = nullptr;
}

SpaceTreeNode::~SpaceTreeNode()
{
	delete left;
	delete right;
}
