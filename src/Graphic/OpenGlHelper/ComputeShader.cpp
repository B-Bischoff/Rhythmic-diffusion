#include "./ComputeShader.hpp"

ComputeShader::ComputeShader()
{
	return;
}

ComputeShader::~ComputeShader()
{
	glDeleteShader(this->_programID);
}

ComputeShader::ComputeShader(const std::string& pathFile, const bool printShaderOnError)
{
	std::string shaderContent = readShaderContent(pathFile);
	CompileShader(shaderContent, printShaderOnError);
	LinkProgram();
}

ComputeShader::ComputeShader(const std::vector<std::string>& pathFiles, const bool printShaderOnError)
{
	std::string shaderContent;
	for (int i = 0; i < (int)pathFiles.size(); i++)
		shaderContent += readShaderContent(pathFiles[i]);

	CompileShader(shaderContent, printShaderOnError);
	LinkProgram();
}

std::string ComputeShader::readShaderContent(const std::string& file)
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
		throw std::invalid_argument("[ComputeShader] Can't open shader " + file);
	}
	return fileContent;
}

void ComputeShader::CompileShader(const std::string& shaderContent, const bool printShaderOnError)
{
	const char* shaderSourcePointer = shaderContent.c_str();

	_shaderID = glCreateShader(GL_COMPUTE_SHADER);
	glShaderSource(_shaderID, 1, &shaderSourcePointer, NULL);
	glCompileShader(_shaderID);

	GLint result = GL_FALSE;
	int infoLogLength = 0;
	glGetShaderiv(_shaderID, GL_COMPILE_STATUS, &result);
	glGetShaderiv(_shaderID, GL_INFO_LOG_LENGTH, &infoLogLength);
	if (infoLogLength > 0)
	{
		std::vector<char> VertexComputeShaderErrorMessage(infoLogLength + 1);
		glGetShaderInfoLog(_shaderID, infoLogLength, NULL, &VertexComputeShaderErrorMessage[0]);

		if (printShaderOnError)
			printShader(shaderContent);

		std::cerr << &VertexComputeShaderErrorMessage[0] << std::endl;
		exit (1);
	}
}

void ComputeShader::LinkProgram()
{
	this->_programID = glCreateProgram();
	glAttachShader(_programID, _shaderID);
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

	glDetachShader(_programID, _shaderID);
	glDeleteShader(_shaderID);
}

void ComputeShader::printShader(const std::string& shaderContent) const
{
	std::istringstream stream(shaderContent);
	std::string line;
	int lineNumber = 1;
	while (std::getline(stream, line))
	{
		std::cout << std::setw(4) << std::left << lineNumber++ << "| ";
		std::cout << line << std::endl;
	}

	std::cout << std::endl << std::endl;
}

GLuint ComputeShader::getProgramID() const
{
	return this->_programID;
}

void ComputeShader::useProgram() const
{
	glUseProgram(this->_programID);
}

void ComputeShader::setBool(const std::string &name, bool value) const
{
	glUniform1i(glGetUniformLocation(_programID, name.c_str()), (int)value);
}
void ComputeShader::setInt(const std::string &name, int value) const
{
	glUniform1i(glGetUniformLocation(_programID, name.c_str()), value);
}
void ComputeShader::setFloat(const std::string &name, float value) const
{
	glUniform1f(glGetUniformLocation(_programID, name.c_str()), value);
}
void ComputeShader::setVec2(const std::string &name, const glm::vec2 &value) const
{
	glUniform2fv(glGetUniformLocation(_programID, name.c_str()), 1, &value[0]);
}
void ComputeShader::setVec2(const std::string &name, float x, float y) const
{
	glUniform2f(glGetUniformLocation(_programID, name.c_str()), x, y);
}
void ComputeShader::setVec3(const std::string &name, const glm::vec3 &value) const
{
	glUniform3fv(glGetUniformLocation(_programID, name.c_str()), 1, &value[0]);
}
void ComputeShader::setVec3(const std::string &name, float x, float y, float z) const
{
	glUniform3f(glGetUniformLocation(_programID, name.c_str()), x, y, z);
}
void ComputeShader::setVec4(const std::string &name, const glm::vec4 &value) const
{
	glUniform4fv(glGetUniformLocation(_programID, name.c_str()), 1, &value[0]);
}
void ComputeShader::setVec4(const std::string &name, float x, float y, float z, float w) const
{
	glUniform4f(glGetUniformLocation(_programID, name.c_str()), x, y, z, w);
}
void ComputeShader::setMat2(const std::string &name, const glm::mat2 &mat) const
{
	glUniformMatrix2fv(glGetUniformLocation(_programID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}
void ComputeShader::setMat3(const std::string &name, const glm::mat3 &mat) const
{
	glUniformMatrix3fv(glGetUniformLocation(_programID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}
void ComputeShader::setMat4(const std::string &name, const glm::mat4 &mat) const
{
	glUniformMatrix4fv(glGetUniformLocation(_programID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}

