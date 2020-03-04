#include "Material.h"

Material::Material(GLuint programInd, DefinedMaterial type)
{
	program = programInd;
	colorLoc = glGetUniformLocation(program, "color");
	ambientLoc = glGetUniformLocation(program, "ambient");
	diffuseLoc = glGetUniformLocation(program, "diffuse");
	specularLoc = glGetUniformLocation(program, "specular");
	shininessLoc = glGetUniformLocation(program, "shininess");

	// Material for dragon
	color = glm::vec3(0.7f, 0.7f, 0.2f);
	ambient = glm::vec3(0.01f, 0.01f, .0f);
	diffuse = glm::vec3(.0f, .0f, .0f);
	specular = glm::vec3(.9f, .9f, .9f);

	shininess = 0.9f;

	switch (type) {
	case DefinedMaterial::SHINY:
		// Material for dragon
		color = glm::vec3(0.9f, 0.9f, 0.2f);
		ambient = glm::vec3(.05f, .05f, .05f);
		diffuse = glm::vec3(.0f, .0f, .0f);
		specular = glm::vec3(.8f, .8f, .1f);

		shininess = 5.f;
		break;

	case DefinedMaterial::DIFFUSE:
		//material for bear
		color = glm::vec3(0.9f, 0.9f, 0.2f);
		ambient = glm::vec3(.05f, .05f, .05f);
		diffuse = glm::vec3(.1f, .6f, .1f);
		specular = glm::vec3(.0f, .0f, .0f);

		shininess = 0;
		break;

	case DefinedMaterial::REGULAR:
		//TODO: material for bunny
		color = glm::vec3(0.9f, 0.9f, 0.2f);
		ambient = glm::vec3(.05f, .05f, .05f);
		diffuse = glm::vec3(.3f, .2f, .6f);
		specular = glm::vec3(.1f, .1f, .2f);

		shininess = 1;
		break;
	}
}

Material::~Material()
{}

void Material::setShaderProgram(GLuint programInd) {
	//TODO: get all those locations based on passed-in programInd
	// Should be called when 'n' - switch shading mode is called.
	program = programInd;
	colorLoc = glGetUniformLocation(program, "color");
	ambientLoc = glGetUniformLocation(program, "ambient");
	diffuseLoc = glGetUniformLocation(program, "diffuse");
	specularLoc = glGetUniformLocation(program, "specular");
	shininessLoc = glGetUniformLocation(program, "shininess");
}

void Material::sendToShader()
{
	//TODO: populate different values to corresponding uniform variables in shader
	glUseProgram(program);

	glUniform3fv(colorLoc, 1, glm::value_ptr(color));
	glUniform3fv(ambientLoc, 1, glm::value_ptr(ambient));
	glUniform3fv(diffuseLoc, 1, glm::value_ptr(diffuse));
	glUniform3fv(specularLoc, 1, glm::value_ptr(specular));
	glUniform1f(shininessLoc, shininess);
}
