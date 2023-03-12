#pragma once

#include "../../../inc/include.hpp"

class ComputeShader {
private:

	GLuint _programID;
	GLuint _shaderID;

	std::string readShaderContent(const std::string& file);
	void CompileShader(const std::string& shaderContent);
	void LinkProgram();

public:
	ComputeShader();
	ComputeShader(const std::string& pathFile);
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
