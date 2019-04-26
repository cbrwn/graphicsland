#include "litshader.h"

LitShader::LitShader() : ShaderProgram() {}
LitShader::~LitShader() {}

/*
	@brief Sets the values of an individual light
	@param Index of the light to set
	@param Light struct containing all new settings for the light
	@return Pointer to this object, for use in chaining
*/
LitShader* LitShader::setLight(unsigned int index,
	LitShader::Light const& info)
{
	setLightPos(index, info.position);
	setLightDiffuse(index, info.diffuse);
	setLightSpecular(index, info.specular);
	return this;
}

/*
	@brief Sets the intensity of the (white) ambient light
	@param Intensity of the ambient light
	@return Pointer to this object, for use in chaining
*/
LitShader* LitShader::setAmbientLight(float f)
{
	bindUniform("ambientLight", f);
	return this;
}

/*
	@brief Sets the position of a specific light
	@param Index of the light to change
	@param World position of the light
	@return Pointer to this object, for use in chaining
*/
LitShader* LitShader::setLightPos(unsigned int index, glm::vec3 const& p)
{
	bindUniform(getUniformName("pos", index).c_str(), p);
	return this;
}

/*
	@brief Sets the diffuse colour of a specific light
	@param Index of the light to change
	@param Colour of the light
	@return Pointer to this object, for use in chaining
*/
LitShader* LitShader::setLightDiffuse(unsigned int index,
	glm::vec4 const& p)
{
	bindUniform(getUniformName("diffuse", index).c_str(), p);
	return this;
}

/*
	@brief Sets the specular colour of a specific light
	@param Index of the light to change
	@param Colour of the specular light
	@return Pointer to this object, for use in chaining
*/
LitShader* LitShader::setLightSpecular(unsigned int index,
	glm::vec4 const& p)
{
	bindUniform(getUniformName("specular", index).c_str(), p);
	return this;
}

/*
	@brief Sets the combined model-view-projection matrix used in the shader
	@param 4x4 MVP matrix
	@return Pointer to this object, for use in chaining
*/
LitShader* LitShader::setMVP(glm::mat4 const& mvp)
{
	bindUniform("MVP", mvp);
	return this;
}

/*
	@brief Sets the view matrix to pass into the shader
	@param 4x4 view matrix
	@return Pointer to this object, for use in chaining
*/
LitShader* LitShader::setViewMatrix(glm::mat4 const& v)
{
	bindUniform("V", v);
	return this;
}

/*
	@brief Sets the model matrix to pass into the shader
	@param 4x4 modelmatrix
	@return Pointer to this object, for use in chaining
*/
LitShader* LitShader::setModelMatrix(glm::mat4 const& m)
{
	bindUniform("M", m);
	return this;
}

/*
	@brief Used internally to reference attributes of specific lights
			in the array inside the shader
	@param Attribute to reference (e.g. 'diffuse')
	@param Index of light to reference
	@return String of the uniform name to reference an attribute
			e.g. Lights[2].diffuse
*/
std::string LitShader::getUniformName(std::string attrib, unsigned int index)
{
	return "lights[" + std::to_string(index) + "]." + attrib;
}

/*
	@brief Sets the number of lights for the shader to use
	@param Number of lights to use
	@return Pointer to this object, for use in chaining
*/
LitShader* LitShader::setLightCount(unsigned int c)
{
	bindUniform("lightCount", c);
	return this;
}
