#pragma once

#include <string>
#include <glm/ext.hpp>

#include "shader.h"

#define LIT_MAX_LIGHTS 8

class LitShader : public ShaderProgram
{
public:
	struct Light
	{
		glm::vec3 position;
		glm::vec4 diffuse;
		glm::vec4 specular;
	};

	LitShader();
	virtual ~LitShader();

	/*
		@brief Sets the number of lights for the shader to use
		@param Number of lights to use
		@return Pointer to this object, for use in chaining
	*/
	LitShader* setLightCount(unsigned int c);
	/*
		@brief Sets the values of an individual light
		@param Index of the light to set
		@param Light struct containing all new settings for the light
		@return Pointer to this object, for use in chaining
	*/
	LitShader* setLight(unsigned int index, LitShader::Light const& info);

	/*
		@brief Sets the intensity of the (white) ambient light
		@param Intensity of the ambient light
		@return Pointer to this object, for use in chaining
	*/
	LitShader* setAmbientLight(float f);

	// set individual parts of lights
	/*
		@brief Sets the position of a specific light
		@param Index of the light to change
		@param World position of the light
		@return Pointer to this object, for use in chaining
	*/
	LitShader* setLightPos(unsigned int index, glm::vec3 const& p);
	/*
		@brief Sets the diffuse colour of a specific light
		@param Index of the light to change
		@param Colour of the light
		@return Pointer to this object, for use in chaining
	*/
	LitShader* setLightDiffuse(unsigned int index, glm::vec4 const& p);
	/*
		@brief Sets the specular colour of a specific light
		@param Index of the light to change
		@param Colour of the specular light
		@return Pointer to this object, for use in chaining
	*/
	LitShader* setLightSpecular(unsigned int index, glm::vec4 const& p);

	/*
		@brief Sets the combined model-view-projection matrix used in the shader
		@param 4x4 MVP matrix
		@return Pointer to this object, for use in chaining
	*/
	LitShader* setMVP(glm::mat4 const& mvp);
	/*
		@brief Sets the view matrix to pass into the shader
		@param 4x4 view matrix
		@return Pointer to this object, for use in chaining
	*/
	LitShader* setViewMatrix(glm::mat4 const& v);
	/*
		@brief Sets the model matrix to pass into the shader
		@param 4x4 modelmatrix
		@return Pointer to this object, for use in chaining
	*/
	LitShader* setModelMatrix(glm::mat4 const& m);

	/*
		@brief Ability to perform actions before drawing happens, such as
				binding custom textures
	*/
	virtual void beforeDraw() {}
private:
	/*
		@brief Used internally to reference attributes of specific lights
				in the array inside the shader
		@param Attribute to reference (e.g. 'diffuse')
		@param Index of light to reference
		@return String of the uniform name to reference an attribute
				e.g. Lights[2].diffuse
	*/
	std::string getUniformName(std::string attrib, unsigned int index);

};
