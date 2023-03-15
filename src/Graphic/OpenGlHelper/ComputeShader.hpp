#pragma once

#include "../../../inc/include.hpp"
#include <iomanip>

class ComputeShader {
private:

	GLuint _programID;
	GLuint _shaderID;

	std::string readShaderContent(const std::string& file);
	void CompileShader(const std::string& shaderContent, const bool printShaderOnError);
	void LinkProgram();

	void printShader(const std::string& shaderContent) const;

public:
	ComputeShader();
	ComputeShader(const std::string& pathFile, const bool printShaderOnError = true);
	ComputeShader(const std::vector<std::string>& pathFiles, const bool printShaderOnError = true);
	~ComputeShader();

	GLuint getProgramID() const;
	void useProgram() const;

	void setBool(const std::string& name, bool value) const;
	void setInt(const std::string& name, int value) const;
	void setFloat(const std::string& name, float value) const;
	void setVec2(const std::string& name, const glm::vec2& value) const;
	void setVec2(const std::string& name, float x, float y) const;
	void setVec3(const std::string& name, const glm::vec3& value) const;
	void setVec3(const std::string& name, float x, float y, float z) const;
	void setVec4(const std::string& name, const glm::vec4& value) const;
	void setVec4(const std::string& name, float x, float y, float z, float w) const;
	void setMat2(const std::string& name, const glm::mat2& mat) const;
	void setMat3(const std::string& name, const glm::mat3& mat) const;
	void setMat4(const std::string& name, const glm::mat4& mat) const;
};
