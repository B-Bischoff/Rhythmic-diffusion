#include "ShaderProgram.hpp"

ShaderProgram::ShaderProgram()
{
	return;
}

ShaderProgram::~ShaderProgram()
{
	glDeleteShader(this->_programID);
}

ShaderProgram::ShaderProgram(std::string vertexPathFile, std::string fragmentPathFile)
{
	this->_vertexShaderID = glCreateShader(GL_VERTEX_SHADER);
	this->_fragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

	std::string vertexShaderContent = readShaderContent(vertexPathFile);
	std::string fragmentShaderContent = readShaderContent(fragmentPathFile);

	CompileShader(vertexShaderContent, this->_vertexShaderID);
	CompileShader(fragmentShaderContent, this->_fragmentShaderID);

	LinkProgram();
}

std::string ShaderProgram::readShaderContent(std::string file)
{
	std::string fileContent;
	std::ifstream fileStream(file, std::ios::in);

	if (fileStream.is_open())
	{
		std::stringstream sstr;
		sstr << fileStream.rdbuf();
		fileContent = sstr.str();
		fileStream.close();
	}
	else
	{
		throw std::invalid_argument("[ShaderProgram] Can't open shader " + file);
	}
	return fileContent;
}

void ShaderProgram::CompileShader(std::string shaderContent, GLuint& shaderID)
{
	char const* shaderSourcePointer = shaderContent.c_str();
	glShaderSource(shaderID, 1, &shaderSourcePointer, NULL);
	glCompileShader(shaderID);

	GLint result = GL_FALSE;
	int infoLogLength;
	glGetShaderiv(shaderID, GL_COMPILE_STATUS, &result);
	glGetShaderiv(shaderID, GL_INFO_LOG_LENGTH, &infoLogLength);
	if (infoLogLength > 0)
	{
		std::vector<char> VertexShaderErrorMessage(infoLogLength + 1);
		glGetShaderInfoLog(shaderID, infoLogLength, NULL, &VertexShaderErrorMessage[0]);
		std::cerr << &VertexShaderErrorMessage[0] << std::endl;
		exit (1);
	}
}

void ShaderProgram::LinkProgram()
{
	this->_programID = glCreateProgram();
	glAttachShader(_programID, _vertexShaderID);
	glAttachShader(_programID, _fragmentShaderID);
	glLinkProgram(_programID);

	GLint result = GL_FALSE;
	GLint infoLogLength;
	glGetProgramiv(_programID, GL_LINK_STATUS, &result);
	glGetProgramiv(_programID, GL_INFO_LOG_LENGTH, &infoLogLength);
	if (infoLogLength > 0)
	{
		std::vector<char> ProgramErrorMessage(infoLogLength + 1);
		glGetProgramInfoLog(_programID, infoLogLength, NULL, &ProgramErrorMessage[0]);
		std::cerr << &ProgramErrorMessage[0] << std::endl;
		exit(1);
	}

	glDetachShader(_programID, _vertexShaderID);
	glDetachShader(_programID, _fragmentShaderID);

	glDeleteShader(_vertexShaderID);
	glDeleteShader(_fragmentShaderID);
}

GLuint ShaderProgram::getProgramID() const
{
	return this->_programID;
}

void ShaderProgram::useProgram()
{
	glUseProgram(this->_programID);
}

void ShaderProgram::setBool(const std::string &name, bool value) const
{
	glUniform1i(glGetUniformLocation(_programID, name.c_str()), (int)value);
}
void ShaderProgram::setInt(const std::string &name, int value) const
{
	glUniform1i(glGetUniformLocation(_programID, name.c_str()), value);
}
void ShaderProgram::setFloat(const std::string &name, float value) const
{
	glUniform1f(glGetUniformLocation(_programID, name.c_str()), value);
}
void ShaderProgram::setVec2(const std::string &name, const glm::vec2 &value) const
{
	glUniform2fv(glGetUniformLocation(_programID, name.c_str()), 1, &value[0]);
}
void ShaderProgram::setVec2(const std::string &name, float x, float y) const
{
	glUniform2f(glGetUniformLocation(_programID, name.c_str()), x, y);
}
void ShaderProgram::setVec3(const std::string &name, const glm::vec3 &value) const
{
	glUniform3fv(glGetUniformLocation(_programID, name.c_str()), 1, &value[0]);
}
void ShaderProgram::setVec3(const std::string &name, float x, float y, float z) const
{
	glUniform3f(glGetUniformLocation(_programID, name.c_str()), x, y, z);
}
void ShaderProgram::setVec4(const std::string &name, const glm::vec4 &value) const
{
	glUniform4fv(glGetUniformLocation(_programID, name.c_str()), 1, &value[0]);
}
void ShaderProgram::setVec4(const std::string &name, float x, float y, float z, float w) const
{
	glUniform4f(glGetUniformLocation(_programID, name.c_str()), x, y, z, w);
}
void ShaderProgram::setMat2(const std::string &name, const glm::mat2 &mat) const
{
	glUniformMatrix2fv(glGetUniformLocation(_programID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}
void ShaderProgram::setMat3(const std::string &name, const glm::mat3 &mat) const
{
	glUniformMatrix3fv(glGetUniformLocation(_programID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}
void ShaderProgram::setMat4(const std::string &name, const glm::mat4 &mat) const
{
	glUniformMatrix4fv(glGetUniformLocation(_programID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}

