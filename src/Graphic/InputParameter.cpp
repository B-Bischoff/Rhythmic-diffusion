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
	_parameters.push_back(0);
}

void InputParameter::changeType(const int& newTypeIndex)
{
	_parameters.clear();
	// DONT USE HARDCODED PATHS
	switch (newTypeIndex)
	{
		case 0: // Number
			_type = InputParameterType::Number;
			_computeShader = ComputeShader("src/shaders/input/number.comp");
			_parameters = std::vector<float>(1, 0);
			break;
		case 1: // Perlin Noise
			_type = InputParameterType::PerlinNoise;
			_computeShader = ComputeShader("src/shaders/input/perlin.comp");
			_parameters = std::vector<float>(6, 0);
			break;
		case 2: // Voronoi
			_type = InputParameterType::Voronoi;
			_computeShader = ComputeShader("src/shaders/input/voronoi.comp");
			_parameters = std::vector<float>(6, 0);
			break;
		default:
			std::cerr << "[InputParameter]: invalid input type selected" << std::endl;
	}
}

void InputParameter::execShader(const int& channel, const glm::vec2& SCREEN_DIMENSION)
{
	_computeShader.useProgram();
	_computeShader.setInt("channel", channel);

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
	if (_parameters.size() < 6)
		return;

	float& scale = _parameters[0];
	float offset[2] { _parameters[1], _parameters[2] };
	float& strengthFactor = _parameters[3];
	bool movingScale = _parameters[4] == 1.0;
	float timeMultiplier = _parameters[5];

	if (movingScale)
		_computeShader.setFloat("scale", sin(glfwGetTime() * timeMultiplier) * scale);
	else
		_computeShader.setFloat("scale", scale);

	_computeShader.setVec2("offset", glm::vec2(offset[0], offset[1]));
	_computeShader.setFloat("strengthFactor", strengthFactor);
}

void InputParameter::applyVoronoiSettings()
{
	if (_parameters.size() < 6)
		return;

	float& scale = _parameters[0];
	float offset[2] { _parameters[1], _parameters[2] };
	float& strengthFactor = _parameters[3];
	bool movingScale = _parameters[4] == 1.0;
	float timeMultiplier = _parameters[5];

	if (movingScale)
		_computeShader.setFloat("scale", sin(glfwGetTime() * timeMultiplier) * scale);
	else
		_computeShader.setFloat("scale", scale);

	_computeShader.setVec2("offset", glm::vec2(offset[0], offset[1]));
	_computeShader.setFloat("strengthFactor", strengthFactor);
}

std::vector<float>& InputParameter::getVectorParameters()
{
	return _parameters;
}

const InputParameterType& InputParameter::getType() const
{
	return _type;
}
