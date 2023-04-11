#pragma once

#include "../../../inc/include.hpp"
#include "../OpenGlHelper/ComputeShader.hpp"
#include "../OpenGlHelper/Texture.hpp"

class PostProcessing {
private:
	ComputeShader _computeShader;
	Texture* _outputTexture;

	ImGradient* _UIGradient; // UI gradient

	/*
	* vec4 is used to keep track of a color and its gradient position as follows
	*	x : Red | y : Blue | z : Green | w : Gradient position (between 0 and 1)
	*/
	std::vector<glm::vec4> _gradient;

	// Smooth gradient transition
	std::vector<glm::vec4> _oldGradient, _newGradient;
	std::chrono::system_clock::time_point _transitionBegining;
	bool _smoothGradientUpdate;

	glm::vec4 _parametersTexturePreview; // Used to visualize RD parameter


public:
	PostProcessing();
	PostProcessing(Texture* texture, const std::string& computeShaderPath);

	void execShader(const glm::vec2& SCREEN_DIMENSIONSS);
	void smoothGradientUpdate();

	void setGradient(const std::vector<glm::vec4>& gradient, const double& duration);
	const std::vector<glm::vec4>& getGradient() const;
	bool getParameterPreview(const int& parameterIndex) const;
	void setParameterPreview(const int& parameterIndex, const bool& value);
	bool getSmoothGradientUpdate();
	void setUIGradient(ImGradient& uiGradient);
};
