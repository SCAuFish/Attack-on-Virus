#include "BezierCurve.h"

int pointCount = 200;

BezierCurve::BezierCurve(glm::vec3 point1, glm::vec3 point2, glm::vec3 point3, glm::vec3 point4) {
	p1 = point1;
	p2 = point2;
	p3 = point3;
	p4 = point4;

	populatePoints(pointCount);

	// Generate a vertex array (VAO) and a vertex buffer objects (VBO).
	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo);

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

	// Unbind from the VBO.
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	// Unbind from the VAO.
	glBindVertexArray(0);
}

BezierCurve::~BezierCurve()
{
}

glm::vec3 BezierCurve::getPoint(float t)
{
	glm::vec3 a = (-p1 + 3.f*p2 - 3.f*p3 + p4);
	glm::vec3 b = (3.f * p1 - 6.f * p2 + 3.f * p3);
	glm::vec3 c = (-3.f * p1 + 3.f * p2);
	glm::vec3 d = p1;
	return (t * t * t) * a + (t * t) * b + t * c + d;
}

void BezierCurve::populatePoints(int pointCount)
{
	for (int i = 0; i < pointCount+1; i++) {
		this->points.push_back(getPoint((i * 1.f) / pointCount));
	}
}

void BezierCurve::draw(glm::mat4 C) {
	this->sendModelToShader(C);

	// Bind to the VAO.
	glBindVertexArray(vao);
	// Set point size.
	glPointSize(2);
	// Draw points 
	//glDrawElements(GL_LINE_STRIP, vertices.size(), GL_UNSIGNED_INT, 0);
	glDrawArrays(GL_LINE_STRIP, 0, points.size());
	// Unbind from the VAO.
	glBindVertexArray(0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}
