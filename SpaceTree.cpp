#include "SpaceTree.h"
#include <unordered_set>

// variable for debug
//float x_min = 1000.0;
//int triangle_count = 0;
//std::unordered_set<unsigned int> vertices;

bool debugOn = false;

// Build KD-Tree
// Sort based on triangle's mid-point coordinate, get bounding box corners recursively
SpaceTreeNode* SpaceTree::buildKDTree(const std::vector<glm::vec3>& points, std::vector<std::vector<unsigned int>>& triangles,
	int dim, int left, int right)
{
	// Base Case should be one triangle left as long as there are more than one element in the tree
	//if (left == right)
	//	return nullptr;
	if (left + 1 == right) {
		SpaceTreeNode* leaf = new SpaceTreeNode(points);
		leaf->left = nullptr;
		leaf->right = nullptr;
		leaf->triangle = triangles[left];

		// Set bouding box of the triangle
		leaf->x_min = std::min({points[leaf->triangle[0]][0], points[leaf->triangle[1]][0], points[leaf->triangle[2]][0]});
		leaf->x_max = std::max({points[leaf->triangle[0]][0], points[leaf->triangle[1]][0], points[leaf->triangle[2]][0]});

		leaf->y_min = std::min({points[leaf->triangle[0]][1], points[leaf->triangle[1]][1], points[leaf->triangle[2]][1]});
		leaf->y_max = std::max({points[leaf->triangle[0]][1], points[leaf->triangle[1]][1], points[leaf->triangle[2]][1]});
							   																							
		leaf->z_min = std::min({points[leaf->triangle[0]][2], points[leaf->triangle[1]][2], points[leaf->triangle[2]][2]});
		leaf->z_max = std::max({points[leaf->triangle[0]][2], points[leaf->triangle[1]][2], points[leaf->triangle[2]][2]});
	
		return leaf;
	}

	int mid = (left + right) / 2;
	// Sort based on dimension
	std::sort(triangles.begin() + left, triangles.begin() + right, 
		[&](const auto& lhs, const auto& rhs) {
		// sort based on mid-point coordinate, hopefully this is 
		float left_mid = (points[lhs[0]][dim] + points[lhs[1]][dim] + points[lhs[2]][dim]);
		float right_mid = (points[rhs[0]][dim] + points[rhs[1]][dim] + points[rhs[2]][dim]);
		return left_mid < right_mid;
	});

	SpaceTreeNode* result = new SpaceTreeNode(points);
	result->left = buildKDTree(points, triangles, (dim + 1) % 3, left, mid);
	result->right = buildKDTree(points, triangles, (dim + 1) % 3, mid, right);

	// Set bounding box for vertices below result node
	result->x_max = std::max(result->left->x_max, result->right->x_max);
	result->x_min = std::min(result->left->x_min, result->right->x_min);

	result->y_max = std::max(result->left->y_max, result->right->y_max);
	result->y_min = std::min(result->left->y_min, result->right->y_min);

	result->z_max = std::max(result->left->z_max, result->right->z_max);
	result->z_min = std::min(result->left->z_min, result->right->z_min);

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

bool SpaceTree::bbIntersect(SpaceTreeNode* lhs, SpaceTreeNode* rhs, const glm::mat4& lhsModel, const glm::mat4& rhsModel)
{
	std::vector<glm::vec3> lhs_corners = constructMinMax(lhs, lhsModel);
	std::vector<glm::vec3> rhs_corners = constructMinMax(rhs, rhsModel);
	glm::vec3 lhs_min = lhs_corners[0];
	glm::vec3 lhs_max = lhs_corners[1];
	glm::vec3 rhs_min = rhs_corners[0];
	glm::vec3 rhs_max = rhs_corners[1];

	// printf("lhs: (%f, %f, %f) - (%f, %f, %f)\n", lhs_min[0], lhs_min[1], lhs_min[2], lhs_max[0], lhs_max[1], lhs_max[2]);
	// printf("rhs: (%f, %f, %f) - (%f, %f, %f)\n", rhs_min[0], rhs_min[1], rhs_min[2], rhs_max[0], rhs_max[1], rhs_max[2]);
	return (lhs_max[0] > rhs_min[0] && lhs_min[0] < rhs_max[0]) 
		&& (lhs_max[1] > rhs_min[1] && lhs_min[1] < rhs_max[1])
		&& (lhs_max[2] > rhs_min[2] && lhs_min[2] < rhs_max[2]);
}

bool SpaceTree::intersectHelper(SpaceTreeNode* lhs, SpaceTreeNode* rhs, 
	const glm::mat4& lhsModel, const glm::mat4& rhsModel, 
	std::vector<unsigned int>& lhs_triangles, std::vector<unsigned int>& rhs_triangles)
{
	// base case : collision impossible
	if (this->bbIntersect(lhs, rhs, lhsModel, rhsModel) == false) {
		return false;
	}
	// Base Case : single triangle (leaf node)
	if (lhs->left == nullptr && rhs->left == nullptr) {
		// There is no node with only one child, so this case means leaf node
		return triangleIntersect(lhs, rhs, lhsModel, rhsModel, lhs_triangles, rhs_triangles);
	}

	// Recursive Case: go to different subtrees
	if (lhs->left == nullptr) {
		if (debugOn) {
			// get all triangle version
			bool result_left = this->intersectHelper(lhs, rhs->left, lhsModel, rhsModel, lhs_triangles, rhs_triangles);
			bool result_right = this->intersectHelper(lhs, rhs->right, lhsModel, rhsModel, lhs_triangles, rhs_triangles);
			return result_left || result_right;
		}
		else {
			// fast version
			return this->intersectHelper(lhs, rhs->left, lhsModel, rhsModel, lhs_triangles, rhs_triangles)
				|| this->intersectHelper(lhs, rhs->right, lhsModel, rhsModel, lhs_triangles, rhs_triangles);
		}
	}
	if (rhs->left == nullptr) {
		if (debugOn) {
			bool result_left = this->intersectHelper(lhs->left, rhs, lhsModel, rhsModel, lhs_triangles, rhs_triangles);
			bool result_right = this->intersectHelper(lhs->right, rhs, lhsModel, rhsModel, lhs_triangles, rhs_triangles);
			return result_left || result_right;
		}
		else{
			// fast version
			return this->intersectHelper(lhs->left, rhs, lhsModel, rhsModel, lhs_triangles, rhs_triangles)
				|| this->intersectHelper(lhs->right, rhs, lhsModel, rhsModel, lhs_triangles, rhs_triangles);
		}
	}

	if (debugOn){
		bool result1 = this->intersectHelper(lhs->left, rhs->left, lhsModel, rhsModel, lhs_triangles, rhs_triangles);
		bool result2 = this->intersectHelper(lhs->left, rhs->right, lhsModel, rhsModel, lhs_triangles, rhs_triangles);
		bool result3 = this->intersectHelper(lhs->right, rhs->left, lhsModel, rhsModel, lhs_triangles, rhs_triangles);
		bool result4 = this->intersectHelper(lhs->right, rhs->right, lhsModel, rhsModel, lhs_triangles, rhs_triangles);
		return result1 || result2 || result3 || result4;
	}
	else {
		// fast version
		return this->intersectHelper(lhs->left, rhs->left, lhsModel, rhsModel, lhs_triangles, rhs_triangles)
			|| this->intersectHelper(lhs->left, rhs->right, lhsModel, rhsModel, lhs_triangles, rhs_triangles)
			|| this->intersectHelper(lhs->right, rhs->left, lhsModel, rhsModel, lhs_triangles, rhs_triangles)
			|| this->intersectHelper(lhs->right, rhs->right, lhsModel, rhsModel, lhs_triangles, rhs_triangles);
	}
}

bool SpaceTree::triangleIntersect(SpaceTreeNode* lhs, SpaceTreeNode* rhs, const glm::mat4& lhsModel, const glm::mat4& rhsModel,
	std::vector<unsigned int>& lhs_triangles, std::vector<unsigned int>& rhs_triangles)
{
	// POC : using bbIntersect
	bool result = false;
	// result = this->bbIntersect(lhs, rhs, lhsModel, rhsModel);

	// Using Raabe algorithm proposed in 2009
	std::vector<glm::vec3> p = { lhs->points[lhs->triangle[0]], lhs->points[lhs->triangle[1]], lhs->points[lhs->triangle[2]] };
	std::vector<glm::vec3> u = { p[1] - p[0], p[2] - p[1], p[0] - p[2] };

	std::vector<glm::vec3> q = { rhs->points[rhs->triangle[0]], rhs->points[rhs->triangle[1]], rhs->points[rhs->triangle[2]] };
	std::vector<glm::vec3> v = { q[1] - q[0], q[2] - q[1], q[0] - q[2] };

	std::vector<glm::vec3> l;
	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 3; j++) {
			l.push_back(glm::cross(u[i], v[j]));
		}
	}

	for (glm::vec3 l : l) {
		float t1, t2, s1, s2;
		t1 = std::min({ glm::dot(p[0], l), glm::dot(p[1], l), glm::dot(p[2], l) });
		t2 = std::max({ glm::dot(p[0], l), glm::dot(p[1], l), glm::dot(p[2], l) });
		s1 = std::min({ glm::dot(q[0], l), glm::dot(q[1], l), glm::dot(q[2], l) });
		s2 = std::max({ glm::dot(q[0], l), glm::dot(q[1], l), glm::dot(q[2], l) });

		if (t1 < s2 && s1 < t2) {
			result = true;
			break;
		}
	}
	// TODO: implementing Raabe algorithm
	// add intersected triangle meshes into vector
	if (result) {
		lhs_triangles.push_back(lhs->triangle[0]);
		lhs_triangles.push_back(lhs->triangle[1]);
		lhs_triangles.push_back(lhs->triangle[2]);

		rhs_triangles.push_back(rhs->triangle[0]);
		rhs_triangles.push_back(rhs->triangle[1]);
		rhs_triangles.push_back(rhs->triangle[2]);
	}
	
	return result;
}

SpaceTree::SpaceTree(const std::vector<glm::vec3>& points, std::vector<unsigned int> triangles) :
	points(points)
{
	std::vector<std::vector<unsigned int>> triangle_vector;
	for (unsigned int i = 0; i < (triangles.size() / 3); i++) {
		triangle_vector.push_back({ triangles[3 * i], triangles[3 * i + 1], triangles[3 * i + 2] });
	}
	root = buildKDTree(points, triangle_vector, 0, 0, triangle_vector.size());
}

SpaceTree::~SpaceTree()
{
}

bool SpaceTree::intersectWith(SpaceTree* other, const glm::mat4& model, const glm::mat4& otherModel,
	std::vector<unsigned int>& lhs_triangles, std::vector<unsigned int>& rhs_triangles)
{
	return this->intersectHelper(this->root, other->root, model, otherModel, lhs_triangles, rhs_triangles);
}
