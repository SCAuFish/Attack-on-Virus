#pragma once
#ifndef _BEZIERCURVE_H_
#define _BEZIERCURVE_H_
#include "Geometry.h"

extern int pointCount;

class BezierCurve :
	public Geometry
{
public:
	GLuint vao, vbo;
	glm::vec3 p1, p2, p3, p4;
	std::vector<glm::vec3> points;
	std::vector<int> vertices;
	BezierCurve(glm::vec3 point1, glm::vec3 point2, glm::vec3 point3, glm::vec3 point4);
	~BezierCurve();

	glm::vec3 getPoint(float t);
	void populatePoints(int pointCount);
	void draw(glm::mat4 C);
};

#endif