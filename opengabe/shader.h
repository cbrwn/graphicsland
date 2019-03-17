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

protected:
	ShaderStage m_stage;
	unsigned int m_handle;

private:
	unsigned int makeHandle(ShaderStage stage);
};

class ShaderProgram
{
public:
	ShaderProgram();
	~ShaderProgram();

	bool loadShader(ShaderStage stage, const char* filename);
	bool createShader(ShaderStage stage, const char* source);
	void attachShader(Shader* shader);

	bool link();

	void use();

	inline unsigned int getHandle() const { return m_handle; }

	int getUniform(const char* name);

	// bind uniforms!
	void bindUniform(int ID, const float value);
	void bindUniform(int ID, const glm::vec3& value);
	void bindUniform(int ID, const glm::mat4& value);

private:
	unsigned int m_handle;
	Shader* m_shaders[(int)ShaderStage::COUNT];
};