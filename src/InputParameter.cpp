#include "InputParameter.hpp"

InputParameter::InputParameter()
	: _type(InputParameterType::Number)
{
}

InputParameter::~InputParameter()
{
}

InputParameter::InputParameter(Texture* texture)
	: _parametersTexture(texture)
{
	changeType(InputParameterType::Number);
}

void InputParameter::changeType(const InputParameterType& newType)
{
	changeType((int)newType);
}

void InputParameter::changeType(const int& newTypeIndex)
{
	_parameters.clear();
	// DONT USE HARDCODED PATHS
	switch (newTypeIndex)
	{
		case 0: // Number
			_type = InputParameterType::Number;
			_computeShader = ComputeShader("src/shaders/numberInput.cs");
			break;
		case 1: // Perlin Noise
			_type = InputParameterType::PerlinNoise;
			_computeShader = ComputeShader("src/shaders/input1.cs");
			break;
		case 2: // Voronoi
			_type = InputParameterType::Voronoi;
			_computeShader = ComputeShader("src/shaders/voronoi.cs");
			break;
		default:
			return;
	}
}

void InputParameter::execShader(const glm::vec4& channels, const glm::vec2& SCREEN_DIMENSION)
{
	_computeShader.useProgram();
	_computeShader.setVec4("channels", channels);

	applyParameterSettings();

	glDispatchCompute(ceil(SCREEN_DIMENSION.x/8),ceil(SCREEN_DIMENSION.y/4),1);
	glMemoryBarrier(GL_ALL_BARRIER_BITS);
}

void InputParameter::applyParameterSettings()
{
	switch ((int)_type)
	{
		case 0: applyNumberSettings(); break;
		case 1: applyPerlinNoiseSettings(); break;
		case 2: applyVoronoiSettings(); break;
		default: return;
	}
}

void InputParameter::applyNumberSettings()
{
	if (_parameters.size() >= 1)
		_computeShader.setFloat("value", _parameters[0]);
}

void InputParameter::applyPerlinNoiseSettings()
{
	_computeShader.setFloat("t", glfwGetTime());
}

void InputParameter::applyVoronoiSettings()
{
	// Set voronoi scale
	// offset
	// moving with time
}

std::vector<float>& InputParameter::getVectorParameters()
{
	return _parameters;
}

const InputParameterType& InputParameter::getType() const
{
	return _type;
}
