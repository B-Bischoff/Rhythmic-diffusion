#pragma once

#include "../../../inc/include.hpp"
#include "../OpenGlHelper/ComputeShader.hpp"
#include "../OpenGlHelper/Texture.hpp"

class InitialConditions {
private:
	ComputeShader _computeShader;

	float _radius;

public:
	InitialConditions();
	InitialConditions(const std::string& computeShaderPath);

	void changeShader(const std::string& computeShaderPath);

	void execShader(const glm::vec2& screenDimensions);

	void setRadius(const float& radius);
};
