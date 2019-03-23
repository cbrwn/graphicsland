#include "phong.h"

#include "shader.h"

PhongShader::PhongShader()
{
	m_program = new ShaderProgram();
	m_program->loadShader(ShaderStage::VERTEX, "shaders/textured.vert");
	m_program->loadShader(ShaderStage::FRAGMENT, "shaders/textured.frag");
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
