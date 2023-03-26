#pragma once

#include "../../../inc/include.hpp"
#include "../OpenGlHelper/ComputeShader.hpp"
#include "../OpenGlHelper/Texture.hpp"

class PostProcessing {
private:
	ComputeShader _computeShader;
	Texture* _outputTexture;

	/* 
	 * vec4 is used to keep track of a color and its gradient position as follows
	 *	x : Red | y : Blue | z : Green | w : Gradient position (between 0 and 1)
	*/
	std::vector<glm::vec4> _gradient; 
	glm::vec4 _parametersTexturePreview; // Used to visualize RD parameter

public:
	PostProcessing();
	PostProcessing(Texture* texture, const std::string& computeShaderPath);

	void execShader(const glm::vec2& SCREEN_DIMENSIONSS);

	void setGradient(const std::vector<glm::vec4>& gradient);
	bool getParameterPreview(const int& parameterIndex) const;
	void setParameterPreview(const int& parameterIndex, const bool& value);
};
