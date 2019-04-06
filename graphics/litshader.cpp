#include "litshader.h"

LitShader::LitShader() : ShaderProgram() {}

LitShader::~LitShader() {}

LitShader* LitShader::setLight(unsigned int index,
	LitShader::Light const& info)
{
	setLightPos(index, info.position);
	setLightDiffuse(index, info.diffuse);
	setLightSpecular(index, info.specular);
	return this;
}

LitShader* LitShader::setAmbientLight(float f)
{
	bindUniform("ambientLight", f);
	return this;
}

LitShader* LitShader::setLightPos(unsigned int index, glm::vec3 const& p)
{
	bindUniform(getUniformName("pos", index).c_str(), p);
	return this;
}

LitShader* LitShader::setLightDiffuse(unsigned int index,
	glm::vec4 const& p)
{
	bindUniform(getUniformName("diffuse", index).c_str(), p);
	return this;
}

LitShader* LitShader::setLightSpecular(unsigned int index,
	glm::vec4 const& p)
{
	bindUniform(getUniformName("specular", index).c_str(), p);
	return this;
}

LitShader* LitShader::setMVP(glm::mat4 const& mvp)
{
	bindUniform("MVP", mvp);
	return this;
}

LitShader* LitShader::setViewMatrix(glm::mat4 const& v)
{
	bindUniform("V", v);
	return this;
}

LitShader* LitShader::setModelMatrix(glm::mat4 const& m)
{
	bindUniform("M", m);
	return this;
}

std::string LitShader::getUniformName(std::string attrib, unsigned int index)
{
	return "lights[" + std::to_string(index) + "]." + attrib;
}

LitShader* LitShader::setLightCount(unsigned int c)
{
	bindUniform("lightCount", c);
	return this;
}
