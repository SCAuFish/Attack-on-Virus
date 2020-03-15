#include "SpaceTree.h"

SpaceTreeNode* SpaceTree::buildKDTree(std::vector<glm::vec3>& v, int dim, int left, int right)
{
	// Base Case
	if (left == right)
		return nullptr;

	int mid = (left + right) / 2;
	// Sort based on dimension
	std::sort(v.begin() + left, v.begin() + right, [&](const auto& lhs, const auto& rhs) {
		return lhs[dim] < rhs[dim];
	});

	SpaceTreeNode* result = new SpaceTreeNode(v[mid]);
	result->left = buildKDTree(v, (dim + 1) % 3, left, mid);
	result->right = buildKDTree(v, (dim + 1) % 3, mid + 1, right);

	// Set bounding box for vertices below result node
	result->x_min = result->x_max = v[mid][0];
	result->y_min = result->y_max = v[mid][1];
	result->z_min = result->z_max = v[mid][2];

	if (result->left != nullptr) {
		result->x_max = std::max(result->x_max, result->left->x_max);
		result->x_min = std::min(result->x_min, result->left->x_min);

		result->y_max = std::max(result->y_max, result->left->y_max);
		result->y_min = std::min(result->y_min, result->left->y_min);

		result->z_max = std::max(result->z_max, result->left->z_max);
		result->z_min = std::min(result->z_min, result->left->z_min);
	}

	if (result->right != nullptr) {
		result->x_max = std::max(result->x_max, result->right->x_max);
		result->x_min = std::min(result->x_min, result->right->x_min);

		result->y_max = std::max(result->y_max, result->right->y_max);
		result->y_min = std::min(result->y_min, result->right->y_min);

		result->z_max = std::max(result->z_max, result->right->z_max);
		result->z_min = std::min(result->z_min, result->right->z_min);
	}

	return result;
}

std::vector<glm::vec3> constructMinMax(SpaceTreeNode* root, const glm::mat4& model) {
	float x_min = root->x_min;
	float x_max = root->x_max;
	float y_min = root->y_min;
	float y_max = root->y_max;
	float z_min = root->z_min;
	float z_max = root->z_max;

	std::vector<glm::vec3> candidate;
	candidate.push_back((glm::vec3)(model * glm::vec4(x_min, y_min, z_min, 1.0f)));
	candidate.push_back((glm::vec3)(model * glm::vec4(x_min, y_min, z_max, 1.0f)));
	candidate.push_back((glm::vec3)(model * glm::vec4(x_min, y_max, z_min, 1.0f)));
	candidate.push_back((glm::vec3)(model * glm::vec4(x_min, y_max, z_max, 1.0f)));
	candidate.push_back((glm::vec3)(model * glm::vec4(x_max, y_min, z_min, 1.0f)));
	candidate.push_back((glm::vec3)(model * glm::vec4(x_max, y_min, z_max, 1.0f)));
	candidate.push_back((glm::vec3)(model * glm::vec4(x_max, y_max, z_min, 1.0f)));
	candidate.push_back((glm::vec3)(model * glm::vec4(x_max, y_max, z_max, 1.0f)));

	glm::vec3 minCorner = candidate[0];
	glm::vec3 maxCorner = candidate[0];
	for (glm::vec3 vertex : candidate) {
		minCorner[0] = std::min(vertex[0], minCorner[0]);
		minCorner[1] = std::min(vertex[1], minCorner[1]);
		minCorner[2] = std::min(vertex[2], minCorner[2]);

		maxCorner[0] = std::max(vertex[0], maxCorner[0]);
		maxCorner[1] = std::max(vertex[1], maxCorner[1]);
		maxCorner[2] = std::max(vertex[2], maxCorner[2]);
	}

	std::vector<glm::vec3> result = { minCorner, maxCorner };
	return result;
}

bool SpaceTree::bbIntersect(SpaceTreeNode* lhs, SpaceTreeNode* rhs, int dim, const glm::mat4& lhsModel, const glm::mat4& rhsModel)
{
	std::vector<glm::vec3> lhs_corners = constructMinMax(lhs, lhsModel);
	std::vector<glm::vec3> rhs_corners = constructMinMax(rhs, rhsModel);
	glm::vec3 lhs_min = lhs_corners[0];
	glm::vec3 lhs_max = lhs_corners[1];
	glm::vec3 rhs_min = rhs_corners[0];
	glm::vec3 rhs_max = rhs_corners[1];

	// printf("lhs: (%f, %f, %f) - (%f, %f, %f)\n", lhs_min[0], lhs_min[1], lhs_min[2], lhs_max[0], lhs_max[1], lhs_max[2]);
	// printf("rhs: (%f, %f, %f) - (%f, %f, %f)\n", rhs_min[0], rhs_min[1], rhs_min[2], rhs_max[0], rhs_max[1], rhs_max[2]);
	return lhs_max[dim] > rhs_min[dim] && lhs_min[dim] < rhs_max[dim];
}

bool SpaceTree::intersectHelper(SpaceTreeNode* lhs, SpaceTreeNode* rhs, int dim, int levelLimit,
	const glm::mat4& lhsModel, const glm::mat4& rhsModel)
{
	// std::cout << levelLimit << std::endl;
	//if (lhs == nullptr || rhs == nullptr) {
	//	std::cout << "return true because of nullptr" << std::endl;
	//	return true;
	//}
	if (levelLimit == 0 ||
		((lhs->left == nullptr || lhs->right == nullptr) && (rhs->left == nullptr || rhs->right == nullptr))) {
		return (this->bbIntersect(lhs, rhs, 0, lhsModel, rhsModel) &&
			this->bbIntersect(lhs, rhs, 1, lhsModel, rhsModel) &&
			this->bbIntersect(lhs, rhs, 2, lhsModel, rhsModel));
	}

	if (this->bbIntersect(lhs, rhs, dim, lhsModel, rhsModel)) {
		if ((lhs->left == nullptr || lhs->right == nullptr) && (rhs->left != nullptr && rhs->right != nullptr)) {
			return this->intersectHelper(lhs, rhs->left, (dim + 1) % 3, levelLimit - 1, lhsModel, rhsModel) ||
				this->intersectHelper(lhs, rhs->right, (dim + 1) % 3, levelLimit - 1, lhsModel, rhsModel);
		}
		else if ((rhs->left == nullptr || rhs->right == nullptr) && (lhs->left != nullptr && lhs->right != nullptr)) {
			return this->intersectHelper(lhs->left, rhs, (dim + 1) % 3, levelLimit - 1, lhsModel, rhsModel) ||
				this->intersectHelper(lhs->right, rhs, (dim + 1) % 3, levelLimit - 1, lhsModel, rhsModel);
		}
		else {
			return this->intersectHelper(lhs->left, rhs->left, (dim + 1) % 3, levelLimit - 1, lhsModel, rhsModel) ||
				this->intersectHelper(lhs->left, rhs->right, (dim + 1) % 3, levelLimit - 1, lhsModel, rhsModel) ||
				this->intersectHelper(lhs->right, rhs->left, (dim + 1) % 3, levelLimit - 1, lhsModel, rhsModel) ||
				this->intersectHelper(lhs->right, rhs->right, (dim + 1) % 3, levelLimit - 1, lhsModel, rhsModel);
		}
	}
	else {
		return false;
	}
}

SpaceTree::SpaceTree(std::vector<glm::vec3> vs)
{
	root = buildKDTree(vs, 0, 0, vs.size());
}

SpaceTree::~SpaceTree()
{
}

bool SpaceTree::intersectWith(SpaceTree* other, const glm::mat4& model, const glm::mat4& otherModel)
{
	return this->intersectHelper(this->root, other->root, 0, 10, model, otherModel);
}
