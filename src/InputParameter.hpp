#pragma once

#include "../inc/include.hpp"
#include "./ComputeShader.hpp"
#include "./Texture.hpp"
#include "./InputParameterSettings.hpp"

class ComputeShader;
class Texture;
class InputParameterSettings;


class InputParameter {
private:

	ComputeShader _computeShader;
	Texture* _parametersTexture;

	void applyParameterSettings(InputParameterSettings* settings);

	void applyNumberInput(struct NumberInput* numberInput);

public:
	InputParameter();
	~InputParameter();
	InputParameter(ComputeShader computeShader, Texture* texture);

	void execShader(const glm::vec4& channels, const glm::vec2& SCREEN_DIMENSION, InputParameterSettings* settings);
};
