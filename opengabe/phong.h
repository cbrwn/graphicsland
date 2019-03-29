#pragma once

#include <string>
#include <glm/ext.hpp>

#include "shader.h"

#define PHONG_MAX_LIGHTS 8

class PhongShader : public ShaderProgram
{
public:
	PhongShader(bool textured = false);

	struct Light
	{
		glm::vec3 position;
		glm::vec4 diffuse;
		glm::vec4 specular;
	};

	// set the number of lights to use
	PhongShader* setLightCount(unsigned int c);
	// set every part of a light
	PhongShader* setLight(unsigned int index, PhongShader::Light const& info);

	PhongShader* setAmbientLight(float f);

	// set individual parts of lights
	PhongShader* setLightPos(unsigned int index, glm::vec3 const& p);
	PhongShader* setLightDiffuse(unsigned int index, glm::vec4 const& p);
	PhongShader* setLightSpecular(unsigned int index, glm::vec4 const& p);

	PhongShader* setMVP(glm::mat4 const& mvp);
	PhongShader* setViewMatrix(glm::mat4 const& v);
	PhongShader* setModelMatrix(glm::mat4 const& m);
private:
	std::string getUniformName(std::string attrib, unsigned int index);

};