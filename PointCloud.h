#ifndef _POINT_CLOUD_H_
#define _POINT_CLOUD_H_

#ifdef __APPLE__
#include <OpenGL/gl3.h>
#else
#include <GL/glew.h>
#endif

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <vector>
#include <string>

#include "Object.h"
#include "Material.h"


class PointCloud : public Object
{
private:
	GLuint vao, vbo, vbo_normal, ebo;
	GLfloat pointSize;

	glm::vec3 spinAxis;
	float spinRate;
	float xShift, yShift, zShift;

	Material* material;
public:
	std::vector<glm::vec3> points, normals;
	std::vector<unsigned int> triangles;
	PointCloud(std::string objFilename, GLfloat pointSize, int objFormat = 0,
		float xShift = .0f, float yShift = .0f, float zShift = .0f);
	~PointCloud();

	void draw();
	void drawComponents(std::vector<unsigned int>& parts);
	void update();

	GLfloat getPointSize();
	void updatePointSize(GLfloat size);
	void setSpinParam(glm::vec3 axis, float spinRate);
	void spin();
	void rotate(float randianAngle, glm::vec3 spinAxis);
	void translate();
	void translate(float xShift, float yShift, float zShift);
	void scale(float xScale, float yScale, float zScale);
	void cancelScaleAndRot();
	void cancelTranslate();

	// Added functions for HW2
	void normalizeModel();

	// Added function for material setting
	void setMaterial(Material::DefinedMaterial type, GLuint programId);
	Material* getMaterial();
};

#endif