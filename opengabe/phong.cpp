#include "phong.h"

PhongShader::PhongShader(bool textured) : ShaderProgram()
{
	if (textured)
	{
		loadShader(ShaderStage::VERTEX, "shaders/phong_textured.vert");
		loadShader(ShaderStage::FRAGMENT, "shaders/phong_textured.frag");
	}
	else
	{
		loadShader(ShaderStage::VERTEX, "shaders/phong.vert");
		loadShader(ShaderStage::FRAGMENT, "shaders/phong.frag");
	}
	link();

	setLightCount(1);

	setLight(0, { {10,10,10}, {1,1,1,1}, {1,1,1,1} });
}

PhongShader* PhongShader::setLight(unsigned int index,
	PhongShader::Light const& info)
{
	setLightPos(index, info.position);
	setLightDiffuse(index, info.diffuse);
	setLightSpecular(index, info.specular);
	return this;
}

PhongShader* PhongShader::setAmbientLight(float f)
{
	bindUniform("ambientLight", f);
	return this;
}

PhongShader* PhongShader::setLightPos(unsigned int index, glm::vec3 const& p)
{
	bindUniform(getUniformName("pos", index).c_str(), p);
	return this;
}

PhongShader* PhongShader::setLightDiffuse(unsigned int index,
	glm::vec4 const& p)
{
	bindUniform(getUniformName("diffuse", index).c_str(), p);
	return this;
}

PhongShader* PhongShader::setLightSpecular(unsigned int index,
	glm::vec4 const& p)
{
	bindUniform(getUniformName("specular", index).c_str(), p);
	return this;
}

PhongShader* PhongShader::setMVP(glm::mat4 const& mvp)
{
	bindUniform("MVP", mvp);
	return this;
}

PhongShader* PhongShader::setViewMatrix(glm::mat4 const& v)
{
	bindUniform("V", v);
	return this;
}

PhongShader* PhongShader::setModelMatrix(glm::mat4 const& m)
{
	bindUniform("M", m);
	return this;
}

std::string PhongShader::getUniformName(std::string attrib, unsigned int index)
{
	return "lights[" + std::to_string(index) + "]." + attrib;
}

PhongShader* PhongShader::setLightCount(unsigned int c)
{
	bindUniform("lightCount", c);
	return this;
}
