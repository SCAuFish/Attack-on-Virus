#ifndef _MATERIAL_H_
#define _MATERIAL_H_

#ifdef __APPLE__
#include <OpenGL/g13.h>
#else
#include <GL/glew.h>
#endif

#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

class Material
{
public:
	enum class DefinedMaterial
	{
		SHINY,
		DIFFUSE,
		REGULAR
	};

	glm::vec3 color;
	glm::vec3 ambient;
	glm::vec3 diffuse;
	glm::vec3 specular;

	float shininess;
	GLuint program;

	GLuint colorLoc;
	GLuint ambientLoc;
	GLuint diffuseLoc;
	GLuint specularLoc;
	GLuint shininessLoc;

	Material(GLuint programInd, DefinedMaterial type);
	~Material();

	void setShaderProgram(GLuint programInd);
	void sendToShader();
};


#endif