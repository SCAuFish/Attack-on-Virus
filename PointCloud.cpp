#include "PointCloud.h"
#include "SpaceTree.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <cmath>
#include <unordered_map>
#include <unordered_set>

/**
 * when objFormat is 1: f v/vt/vn v/vt/vn v/vt/vn
 * when objFormat is 2: f v/vt/vn and contains 4 vertices per surface
 */
PointCloud::PointCloud(std::string objFilename, GLfloat pointSize, int objFormat,
	float xShift, float yShift, float zShift)
	: pointSize(pointSize), xShift(xShift), yShift(yShift), zShift(zShift),
	material(nullptr)
{
	// Init spinAxis and spinRate with default values
	spinAxis = glm::vec3(.0f, .0f, .0f);
	spinRate = 0.1f;
	std::ifstream objFile(objFilename);
	std::string line;

	std::unordered_map<int, int> vertexToNormal;
	std::unordered_map<int, int> vertexToTex;

	while (std::getline(objFile, line)) {
		std::istringstream lineReader(line);
		std::string dataType;
		float  c1, c2, c3;
		int    v1, v2, v3;
		int    n1, n2, n3;
		int    t1, t2, t3;
		char   _;

		lineReader >> dataType;
		if (dataType.compare("v") == 0) {
			lineReader >> c1 >> c2 >> c3;
			points.push_back(glm::vec3(c1, c2, c3));
		}

		if (dataType.compare("vn") == 0) {
			lineReader >> c1 >> c2 >> c3;
			normals.push_back(glm::vec3(c1, c2, c3));
		}

		if (dataType.compare("vt") == 0) {
			lineReader >> c1 >> c2;
			texCoords.push_back(glm::vec2(c1, c2));
		}

		if (dataType.compare("f") == 0) {
			if (objFormat == 0) {
				lineReader >> v1 >> _ >> _ >> v1;
				lineReader >> v2 >> _ >> _ >> v2;
				lineReader >> v3 >> _ >> _ >> v3;

				// Make it 0-based
				triangles.push_back(v1 - 1);
				triangles.push_back(v2 - 1);
				triangles.push_back(v3 - 1);
			}
			else if (objFormat == 1) {
				lineReader >> v1 >> _ >> t1 >> _ >> n1; // v1/vt1/vn1
				lineReader >> v2 >> _ >> t2 >> _ >> n2;
				lineReader >> v3 >> _ >> t3 >> _ >> n3;

				vertexToNormal[v1 - 1] = n1 - 1;
				vertexToNormal[v2 - 1] = n2 - 1;
				vertexToNormal[v3 - 1] = n3 - 1;

				vertexToTex[v1 - 1] = t1 - 1;
				vertexToTex[v2 - 1] = t2 - 1;
				vertexToTex[v3 - 1] = t3 - 1;

				triangles.push_back(v1 - 1);
				triangles.push_back(v2 - 1);
				triangles.push_back(v3 - 1);
			}
		}
	}

	if (objFormat == 1) {
		// TODO: reorder vertex, normal and texCoord
		std::vector<glm::vec3> new_normals;
		std::vector<glm::vec2> new_texCoord;
		for (int i = 0; i < points.size(); i++) {
			new_normals.push_back(normals[vertexToNormal[i]]);
			new_texCoord.push_back(texCoords[vertexToTex[i]]);
		}

		normals = new_normals;
		texCoords = new_texCoord;
		std::cout << texCoords.size() << std::endl;
	}
	// Set the model matrix to an identity matrix. 
	model = glm::mat4(1);
	this->translate();
	this->normalizeModel();
	// Set the color. 
	color = glm::vec3(1, 0, 0);

	// Generate a vertex array (VAO) and a vertex buffer objects (VBO).
	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo);
	glGenBuffers(1, &vbo_normal);
	glGenBuffers(1, &vbo_tex);
	glGenBuffers(1, &ebo);

	// Bind to the VAO.
	// This tells OpenGL which data it should be paying attention to
	glBindVertexArray(vao);

	// Bind to the first VBO. We will use it to store the points.
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	// Pass in the data.
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * points.size(),
		points.data(), GL_STATIC_DRAW);
	// Enable vertex attribute 0. 
	// We will be able to access points through it.
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);

	glBindBuffer(GL_ARRAY_BUFFER, vbo_normal);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * normals.size(),
		normals.data(), GL_STATIC_DRAW);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);

	if (texCoords.size() != 0) {
		glBindBuffer(GL_ARRAY_BUFFER, vbo_tex);
		glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2) * texCoords.size(),
			texCoords.data(), GL_STATIC_DRAW);
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), 0);
	}

	// Pass in triangle meshes
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * triangles.size(),
		triangles.data(), GL_STATIC_DRAW);

	// Unbind from the VBO.
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	// Unbind from the VAO.
	glBindVertexArray(0);
}

PointCloud::~PointCloud()
{
	// Delete the VBO and the VAO.
	// Failure to delete your VAOs, VBOs and other data given to OpenGL
	// is dangerous and may slow your program and cause memory leaks
	glDeleteBuffers(1, &vbo);
	glDeleteBuffers(1, &vbo_normal);
	glDeleteBuffers(1, &ebo);
	glDeleteVertexArrays(1, &vao);

	delete material;
}

void PointCloud::draw()
{
	// Bind to the VAO.
	glBindVertexArray(vao);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	// Set point size.
	glPointSize(pointSize);
	// Draw points 
	// glDrawArrays(GL_POINTS, 0, points.size());
	glBindTexture(GL_TEXTURE_2D, this->textureId);
	glDrawElements(GL_TRIANGLES, triangles.size(), GL_UNSIGNED_INT, 0);
	// Unbind from the VAO.
	glBindVertexArray(0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void PointCloud::drawComponents(std::vector<unsigned int>& parts) {
	// Bind to the VAO.
	glBindVertexArray(vao);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	// Set point size.
	glPointSize(pointSize);
	// Draw points 
	// glDrawArrays(GL_POINTS, 0, points.size());
	// Pass in triangle meshes
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * parts.size(),
		parts.data(), GL_STATIC_DRAW);
	glDrawElements(GL_TRIANGLES, parts.size(), GL_UNSIGNED_INT, 0);
	// Unbind from the VAO.
	glBindVertexArray(0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	// Pass in triangle meshes when drawing during debug mode
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * triangles.size(),
		triangles.data(), GL_STATIC_DRAW);
}

void PointCloud::update()
{
}

GLfloat PointCloud::getPointSize() {
	return this->pointSize;
}

void PointCloud::updatePointSize(GLfloat size)
{
	if (size <= 0) {
		return;
	}
	this->pointSize = size;
}

void PointCloud::setSpinParam(glm::vec3 axis, float spinRate) {
	spinAxis = axis;
	spinRate = spinRate;
}

void PointCloud::spin()
{
	// Update the model matrix by multiplying a rotation matrix
	model = glm::rotate(model, glm::radians(spinRate), spinAxis);
}

void PointCloud::rotate(float radianAngle, glm::vec3 axis)
{
	model = glm::rotate(radianAngle, axis) * model;
}

void PointCloud::translate() {
	// Translate to non-overlapping position
	model = glm::translate(glm::vec3(xShift, yShift, zShift)) * model;
}

void PointCloud::translate(float xShift, float yShift, float zShift) {
	model = glm::translate(glm::vec3(xShift, yShift, zShift)) * model;
}

void PointCloud::scale(float xScale, float yScale, float zScale) {
	model = glm::scale(model, glm::vec3(xScale, yScale, zScale));
}

void PointCloud::cancelScaleAndRot() {
	// Need to test whether this work
	// Reset everything in the first three columns to be as identity
	glm::vec4 translateVec(model[3]);
	model = glm::mat4(1);
	model[3] = translateVec;
}

void PointCloud::cancelTranslate() {
	glm::vec4 translateVec(0, 0, 0, 1);
	model[3] = translateVec;
	// Reset to non-overlapping position
	this->translate();
}

void PointCloud::normalizeModel() {
	// Find the geometric center by calculating the range of x/y/z
	// Recenter it and make sure the largest range is within -1 and 1
	float xMin, yMin, zMin, xMax, yMax, zMax;
	xMin = xMax = points[0][0];
	yMin = yMax = points[0][1];
	zMin = zMax = points[0][2];

	for (glm::vec3 vertex : points) {
		xMin = fminf(xMin, vertex[0]);
		yMin = fminf(yMin, vertex[1]);
		zMin = fminf(zMin, vertex[2]);

		xMax = fmaxf(xMax, vertex[0]);
		yMax = fmaxf(yMax, vertex[1]);
		zMax = fmaxf(zMax, vertex[2]);
	}

	glm::vec3 centerVec((xMin + xMax) / 2, (yMin + yMax) / 2, (zMin + zMax) / 2);
	float maxDiff = fmaxf(fmaxf(xMax - xMin, yMax - yMin), zMax - zMin);

	for (int i = 0; i < points.size(); i++) {
		points[i] = points[i] - centerVec;
		points[i] = points[i] * (2 / maxDiff);
	}
}

void PointCloud::setMaterial(Material::DefinedMaterial type, GLuint programId) {
	material = new Material(programId, type);
}

void PointCloud::setTextureId(unsigned int textureId)
{
	this->textureId = textureId;
}

Material* PointCloud::getMaterial() {
	return material;
}