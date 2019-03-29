#pragma once

#include <string>
#include <glm/ext.hpp>

#include "shader.h"

#define TOON_MAX_LIGHTS 8

class ToonShader: public ShaderProgram
{
public:
	ToonShader(bool textured = false);

	struct Light
	{
		glm::vec3 position;
		glm::vec4 diffuse;
		glm::vec4 specular;
	};

	// set the number of lights to use
	ToonShader* setLightCount(unsigned int c);
	// set every part of a light
	ToonShader* setLight(unsigned int index, ToonShader::Light const& info);

	ToonShader* setAmbientLight(float f);

	// set individual parts of lights
	ToonShader* setLightPos(unsigned int index, glm::vec3 const& p);
	ToonShader* setLightDiffuse(unsigned int index, glm::vec4 const& p);
	ToonShader* setLightSpecular(unsigned int index, glm::vec4 const& p);

	ToonShader* setMVP(glm::mat4 const& mvp);
	ToonShader* setViewMatrix(glm::mat4 const& v);
	ToonShader* setModelMatrix(glm::mat4 const& m);
private:
	std::string getUniformName(std::string attrib, unsigned int index);

};