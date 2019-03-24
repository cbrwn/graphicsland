#include "phong.h"

#include "shader.h"

PhongShader::PhongShader(bool textured)
{
	m_program = new ShaderProgram();
	if (textured)
	{
		m_program->loadShader(ShaderStage::VERTEX, "shaders/phong_textured.vert");
		m_program->loadShader(ShaderStage::FRAGMENT, "shaders/phong_textured.frag");
	}
	else
	{
		m_program->loadShader(ShaderStage::VERTEX, "shaders/phong.vert");
		m_program->loadShader(ShaderStage::FRAGMENT, "shaders/phong.frag");
	}
	m_program->link();

	setLightCount(1);

	setLight(0, { {10,10,10}, {1,1,1,1}, {1,1,1,1} });
}

PhongShader::~PhongShader()
{
	delete m_program;
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
	m_program->bindUniform("ambientLight", f);
	return this;
}

PhongShader* PhongShader::setLightPos(unsigned int index, glm::vec3 const& p)
{
	m_program->bindUniform(getUniformName("pos", index).c_str(), p);
	return this;
}

PhongShader* PhongShader::setLightDiffuse(unsigned int index,
	glm::vec4 const& p)
{
	m_program->bindUniform(getUniformName("diffuse", index).c_str(), p);
	return this;
}

PhongShader* PhongShader::setLightSpecular(unsigned int index,
	glm::vec4 const& p)
{
	m_program->bindUniform(getUniformName("specular", index).c_str(), p);
	return this;
}

PhongShader* PhongShader::setMVP(glm::mat4 const& mvp)
{
	m_program->bindUniform("MVP", mvp);
	return this;
}

PhongShader* PhongShader::setViewMatrix(glm::mat4 const& v)
{
	m_program->bindUniform("V", v);
	return this;
}

PhongShader* PhongShader::setModelMatrix(glm::mat4 const& m)
{
	m_program->bindUniform("M", m);
	return this;
}

std::string PhongShader::getUniformName(std::string attrib, unsigned int index)
{
	return "lights[" + std::to_string(index) + "]." + attrib;
}

PhongShader* PhongShader::use()
{
	m_program->use();
	return this;
}

PhongShader* PhongShader::setLightCount(unsigned int c)
{
	m_program->bindUniform("lightCount", c);
	return this;
}
