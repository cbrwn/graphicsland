#include "toon.h"

ToonShader::ToonShader(bool textured) : ShaderProgram()
{
	loadShader(ShaderStage::VERTEX, "shaders/phong.vert");
	loadShader(ShaderStage::FRAGMENT, "shaders/phong.frag");
	link();

	use();
	bindUniform("textured", textured ? 1U : 0U);

	setLightCount(1);

	setLight(0, { {10,10,10}, {1,1,1,1}, {1,1,1,1} });
}

ToonShader* ToonShader::setLight(unsigned int index,
	ToonShader::Light const& info)
{
	setLightPos(index, info.position);
	setLightDiffuse(index, info.diffuse);
	setLightSpecular(index, info.specular);
	return this;
}

ToonShader* ToonShader::setAmbientLight(float f)
{
	bindUniform("ambientLight", f);
	return this;
}

ToonShader* ToonShader::setLightPos(unsigned int index, glm::vec3 const& p)
{
	bindUniform(getUniformName("pos", index).c_str(), p);
	return this;
}

ToonShader* ToonShader::setLightDiffuse(unsigned int index,
	glm::vec4 const& p)
{
	bindUniform(getUniformName("diffuse", index).c_str(), p);
	return this;
}

ToonShader* ToonShader::setLightSpecular(unsigned int index,
	glm::vec4 const& p)
{
	bindUniform(getUniformName("specular", index).c_str(), p);
	return this;
}

ToonShader* ToonShader::setMVP(glm::mat4 const& mvp)
{
	bindUniform("MVP", mvp);
	return this;
}

ToonShader* ToonShader::setViewMatrix(glm::mat4 const& v)
{
	bindUniform("V", v);
	return this;
}

ToonShader* ToonShader::setModelMatrix(glm::mat4 const& m)
{
	bindUniform("M", m);
	return this;
}

std::string ToonShader::getUniformName(std::string attrib, unsigned int index)
{
	return "lights[" + std::to_string(index) + "]." + attrib;
}

ToonShader* ToonShader::setLightCount(unsigned int c)
{
	bindUniform("lightCount", c);
	return this;
}
