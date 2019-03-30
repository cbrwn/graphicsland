#pragma once

#include <string>
#include <glm/ext.hpp>

#include "shader.h"

#define LIT_MAX_LIGHTS 8

class LitShader : public ShaderProgram
{
public:
	LitShader();
	virtual ~LitShader();

	struct Light
	{
		glm::vec3 position;
		glm::vec4 diffuse;
		glm::vec4 specular;
	};

	// set the number of lights to use
	LitShader* setLightCount(unsigned int c);
	// set every part of a light
	LitShader* setLight(unsigned int index, LitShader::Light const& info);

	LitShader* setAmbientLight(float f);

	// set individual parts of lights
	LitShader* setLightPos(unsigned int index, glm::vec3 const& p);
	LitShader* setLightDiffuse(unsigned int index, glm::vec4 const& p);
	LitShader* setLightSpecular(unsigned int index, glm::vec4 const& p);

	LitShader* setMVP(glm::mat4 const& mvp);
	LitShader* setViewMatrix(glm::mat4 const& v);
	LitShader* setModelMatrix(glm::mat4 const& m);

	virtual void beforeDraw() {}
private:
	std::string getUniformName(std::string attrib, unsigned int index);

};
