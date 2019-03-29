#pragma once

#include <glm/ext.hpp>

enum class ShaderStage : unsigned int
{
	NONE,

	VERTEX,
	TESSELATION_EVAL,
	TESSELATION_CONTROL,
	GEOMETRY,
	FRAGMENT,

	COUNT
};

class Shader
{
public:
	Shader();
	Shader(ShaderStage stage);
	Shader(ShaderStage stage, const char* filename);
	~Shader();

	bool loadShader(ShaderStage stage, const char* filename);
	bool createShader(ShaderStage stage, const char* source);

	inline unsigned int getHandle() const { return m_handle; }
	inline ShaderStage getStage() const { return m_stage; }

private:
	ShaderStage m_stage;
	unsigned int m_handle;

	unsigned int makeHandle(ShaderStage stage);
};

class ShaderProgram
{
public:
	ShaderProgram();
	virtual ~ShaderProgram();

	bool loadShader(ShaderStage stage, const char* filename);
	bool createShader(ShaderStage stage, const char* source);
	void attachShader(Shader* shader);

	bool link();

	void use();

	inline unsigned int getHandle() const { return m_handle; }

	int getUniform(const char* name);

	// bind uniforms!
	// these all return a pointer to this object to make chaining possible
	ShaderProgram* bindUniform(int ID, const float value);
	ShaderProgram* bindUniform(int ID, const glm::vec3& value);
	ShaderProgram* bindUniform(int ID, const glm::vec4& value);
	ShaderProgram* bindUniform(int ID, const glm::mat4& value);
	ShaderProgram* bindUniform(int ID, const unsigned int value);

	// bind using uniform name
	ShaderProgram* bindUniform(const char* name, const float value);
	ShaderProgram* bindUniform(const char* name, const glm::vec3& value);
	ShaderProgram* bindUniform(const char* name, const glm::vec4& value);
	ShaderProgram* bindUniform(const char* name, const glm::mat4& value);
	ShaderProgram* bindUniform(const char* name, const unsigned int value);

private:
	unsigned int m_handle;
	Shader* m_shaders[(int)ShaderStage::COUNT];
};