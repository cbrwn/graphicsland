#pragma once

#include <string>
#include <glm/ext.hpp>

class ShaderProgram;

#define PHONG_MAX_LIGHTS 8

class PhongShader
{
public:
	PhongShader();
	~PhongShader();

	struct Light
	{
		glm::vec3 position;
		glm::vec4 diffuse;
		glm::vec4 specular;
	};

	PhongShader* use();

	// set the number of lights to use
	PhongShader* setLightCount(unsigned int c);
	// set every part of a light
	PhongShader* setLight(unsigned int index, PhongShader::Light const& info);

	// set individual parts of lights
	PhongShader* setLightPos(unsigned int index, glm::vec3 const& p);
	PhongShader* setLightDiffuse(unsigned int index, glm::vec4 const& p);
	PhongShader* setLightSpecular(unsigned int index, glm::vec4 const& p);

	ShaderProgram* getProgram() { return m_program; }
private:
	ShaderProgram* m_program;

	std::string getUniformName(std::string attrib, unsigned int index);

};