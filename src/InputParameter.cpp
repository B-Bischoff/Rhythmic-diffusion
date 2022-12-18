#include "InputParameter.hpp"

InputParameter::InputParameter()
{
}

InputParameter::~InputParameter()
{
}

InputParameter::InputParameter(ComputeShader computeShader, Texture* texture)
	: _computeShader(computeShader), _parametersTexture(texture)
{
}

void InputParameter::execShader(const glm::vec4& channels, const glm::vec2& SCREEN_DIMENSION, InputParameterSettings* settings)
{
	_computeShader.useProgram();
	_computeShader.setVec4("channels", channels);

	applyParameterSettings(settings);

	glDispatchCompute(ceil(SCREEN_DIMENSION.x/8),ceil(SCREEN_DIMENSION.y/4),1);
	glMemoryBarrier(GL_ALL_BARRIER_BITS);
}

void InputParameter::applyParameterSettings(InputParameterSettings* settings)
{
	if (dynamic_cast<NumberInput*>(settings))
	{
		applyNumberInput(dynamic_cast<NumberInput*>(settings));
	}
}

void InputParameter::applyNumberInput(NumberInput* numberInput)
{
	_computeShader.setFloat("value", numberInput->value);
}
