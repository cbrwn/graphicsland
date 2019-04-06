#include "shader.h"

#include <iostream>
#include "gl_core_4_4.h"

//===========================================================================//
// SHADER CLASS
//===========================================================================//

Shader::Shader() : m_stage(ShaderStage::NONE), m_handle(0) {}
Shader::Shader(ShaderStage stage) : m_stage(stage), m_handle(0) {}

Shader::Shader(ShaderStage stage, const char* filename)
	: m_stage(stage), m_handle(0)
{
	loadShader(stage, filename);
}

Shader::~Shader()
{
	glDeleteShader(m_handle);
}

bool Shader::loadShader(ShaderStage stage, const char* filename)
{
	// begin reading from file

	// open file
	FILE* file = nullptr;
	fopen_s(&file, filename, "rb");

	if (!file)
	{
		printf("Couldn't open shader: %s\n", filename);
		return false;
	}

	// get the size of the file
	fseek(file, 0, SEEK_END);
	unsigned int size = ftell(file);

	// allocate space for the file contents
	char* source = new char[size + 1];

	// read from file into source
	fseek(file, 0, SEEK_SET);
	fread_s(source, size + 1, sizeof(char), size, file);
	// make sure to null terminate the string
	source[size] = 0x00;

	// close file
	fclose(file);

	// end reading from file

	// call createShader to create it from the source
	// and store the result so we can delete our allocated string
	bool createResult = createShader(stage, source);

	delete[] source;
	return createResult;
}

bool Shader::createShader(ShaderStage stage, const char* source)
{
	m_stage = stage;
	m_handle = makeHandle(stage);

	glShaderSource(m_handle, 1, &source, 0);
	glCompileShader(m_handle);

	int success;
	glGetShaderiv(m_handle, GL_COMPILE_STATUS, &success);
	if (success == GL_FALSE)
	{
		int errorLength;
		glGetShaderiv(m_handle, GL_INFO_LOG_LENGTH, &errorLength);

		char* error = new char[errorLength];
		glGetShaderInfoLog(m_handle, errorLength, 0, error);
		printf("SHADER COMPILE ERROR: %s\n", error);

		delete[] error;

		return false;
	}
	return true;
}

unsigned int Shader::makeHandle(ShaderStage stage)
{
	GLenum shaderType = -1;

	switch (stage)
	{
	case ShaderStage::VERTEX:
		shaderType = GL_VERTEX_SHADER;
		break;
	case ShaderStage::TESSELATION_EVAL:
		shaderType = GL_TESS_EVALUATION_SHADER;
		break;
	case ShaderStage::TESSELATION_CONTROL:
		shaderType = GL_TESS_CONTROL_SHADER;
		break;
	case ShaderStage::GEOMETRY:
		shaderType = GL_GEOMETRY_SHADER;
		break;
	case ShaderStage::FRAGMENT:
		shaderType = GL_FRAGMENT_SHADER;
		break;
	default:
		return -1;
	}

	return glCreateShader(shaderType);
}

///////////////////////////////////////////////////////////////////////////////
// SHADER CLASS END
///////////////////////////////////////////////////////////////////////////////

//===========================================================================//
// SHADERPROGRAM CLASS
//===========================================================================//

ShaderProgram::ShaderProgram()
	: m_handle(0)
{
	for (int i = 0; i < (int)ShaderStage::COUNT; ++i)
		m_shaders[i] = nullptr;
}

ShaderProgram::~ShaderProgram()
{
	for (int i = 0; i < (int)ShaderStage::COUNT; ++i)
		delete m_shaders[i];
	glDeleteProgram(m_handle);
}

bool ShaderProgram::loadShader(ShaderStage stage, const char* filename)
{
	if (m_shaders[(int)stage])
		printf("SHADER PROGRAM: Already has shader of type %u\n", (int)stage);
	m_shaders[(int)stage] = new Shader();
	return m_shaders[(int)stage]->loadShader(stage, filename);
}

bool ShaderProgram::createShader(ShaderStage stage, const char* source)
{
	if (m_shaders[(int)stage])
		printf("SHADER PROGRAM: Already has shader of type %u\n", (int)stage);
	m_shaders[(int)stage] = new Shader();
	return m_shaders[(int)stage]->createShader(stage, source);
}

void ShaderProgram::attachShader(Shader* shader)
{
	m_shaders[(int)shader->getStage()] = shader;
}

bool ShaderProgram::link()
{
	m_handle = glCreateProgram();

	// attach all shaders
	for (int i = 0; i < (int)ShaderStage::COUNT; ++i)
		if (m_shaders[i])
			glAttachShader(m_handle, m_shaders[i]->getHandle());

	glLinkProgram(m_handle);

	// check for success
	int success;
	glGetProgramiv(m_handle, GL_LINK_STATUS, &success);
	if (success == GL_FALSE)
	{
		int errorLength;
		glGetShaderiv(m_handle, GL_INFO_LOG_LENGTH, &errorLength);

		char* error = new char[errorLength];
		glGetProgramInfoLog(m_handle, errorLength, 0, error);
		printf("SHADER COMPILE ERROR: %s\n", error);

		delete[] error;

		return false;
	}
	return true;
}

void ShaderProgram::use()
{
	glUseProgram(m_handle);
}

int ShaderProgram::getUniform(const char* name)
{
	use();
	return glGetUniformLocation(m_handle, name);
}

//----------------------------
// bind float by ID
//----------------------------
ShaderProgram* ShaderProgram::bindUniform(int ID, const int value)
{
	use();
	glUniform1iv(ID, 1, &value);
	return this;
}

//----------------------------
// bind float by ID
//----------------------------
ShaderProgram* ShaderProgram::bindUniform(int ID, const float value)
{
	use();
	glUniform1fv(ID, 1, &value);
	return this;
}

//----------------------------
// bind texture unit by ID
//----------------------------
ShaderProgram* ShaderProgram::bindTexUniform(int ID, const int value)
{
	use();
	glUniform1i(ID, value);
	return this;
}

//----------------------------
// bind unsigned int by ID
//----------------------------
ShaderProgram* ShaderProgram::bindUniform(int ID, const unsigned int value)
{
	use();
	glUniform1uiv(ID, 1, &value);
	return this;
}

//----------------------------
// bind vec3 by ID
//----------------------------
ShaderProgram* ShaderProgram::bindUniform(int ID, const glm::vec3& value)
{
	use();
	glUniform3f(ID, value.x, value.y, value.z);
	return this;
}

//----------------------------
// bind vec4 by ID
//----------------------------
ShaderProgram* ShaderProgram::bindUniform(int ID, const glm::vec4& value)
{
	use();
	glUniform4f(ID, value.x, value.y, value.z, value.w);
	return this;
}

//----------------------------
// bind mat4 by ID
//----------------------------
ShaderProgram* ShaderProgram::bindUniform(int ID, const glm::mat4& value)
{
	use();
	glUniformMatrix4fv(ID, 1, GL_FALSE, &value[0][0]);
	return this;
}

//----------------------------
// bind int by name
//----------------------------
ShaderProgram* ShaderProgram::bindUniform(const char* name, const int value)
{
	return bindUniform(getUniform(name), value);
}

//----------------------------
// bind float by name
//----------------------------
ShaderProgram* ShaderProgram::bindUniform(const char* name, const float value)
{
	return bindUniform(getUniform(name), value);
}

//----------------------------
// bind texture unit by name
//----------------------------
ShaderProgram* ShaderProgram::bindTexUniform(const char* name, const int value)
{
	return bindTexUniform(getUniform(name), value);
}

//----------------------------
// bind unsigned int by name
//----------------------------
ShaderProgram* ShaderProgram::bindUniform(const char* name, const unsigned int value)
{
	return bindUniform(getUniform(name), value);
}

//----------------------------
// bind vec3 by name
//----------------------------
ShaderProgram* ShaderProgram::bindUniform(const char* name, const glm::vec3& value)
{
	return bindUniform(getUniform(name), value);
}

//----------------------------
// bind vec4 by name
//----------------------------
ShaderProgram* ShaderProgram::bindUniform(const char* name, const glm::vec4& value)
{
	return bindUniform(getUniform(name), value);
}

//----------------------------
// bind mat4 by name
//----------------------------
ShaderProgram* ShaderProgram::bindUniform(const char* name, const glm::mat4& value)
{
	return bindUniform(getUniform(name), value);
}

///////////////////////////////////////////////////////////////////////////////
// SHADERPROGRAM CLASS END
///////////////////////////////////////////////////////////////////////////////
