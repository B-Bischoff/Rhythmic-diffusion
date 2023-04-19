#include "InputParameter.hpp"

InputParameter::InputParameter()
	: _type(InputParameterType::Number)
{
}

InputParameter::~InputParameter()
{
}

InputParameter::InputParameter(Texture* texture, const std::vector<std::string>& computeShadersPath)
	: _parametersTexture(texture)
{
	_computeShader = ComputeShader(computeShadersPath);

	for (int i = 0; i < 4; i++)
		_parameters.push_back(Parameter());

	_needToApplyChanges = true;
}

void InputParameter::changeType(const int& parameterIndex, const InputParameterType& newType)
{
	changeType(parameterIndex, (int)newType);
}

void InputParameter::changeType(const int& parameterIndex, const int& newTypeIndex)
{
	if (parameterIndex < 0 || parameterIndex >= (int)_parameters.size())
	{
		std::cerr << "[InputParameter] invalid parameter index, aborting parameter type modification" << std::endl;
		return;
	}

	std::vector<float>& parameters = _parameters[parameterIndex].parameters;

	parameters.clear();

	_needToApplyChanges = true;

	switch (newTypeIndex)
	{
		case 0: // Number
			parameters = std::vector<float>(1, 0);
			_parameters[parameterIndex].type = Number;
			break;
		case 1: // Perlin Noise
			parameters = std::vector<float>(7, 0);
			parameters[1] = 0.001; // Scale
			_parameters[parameterIndex].type = PerlinNoise;
			break;
		case 2: // Voronoi
			parameters = std::vector<float>(7, 0);
			parameters[1] = 0.001; // Scale
			_parameters[parameterIndex].type = Voronoi;
			break;
		default:
			std::cerr << "[InputParameter] invalid input type selected" << std::endl;
	}
}

void InputParameter::execShader(const glm::vec2& SCREEN_DIMENSION)
{
	if (!_needToApplyChanges) // Check if there is a need to write data on paramTexture
		return;

	bool parameterNeedTime = false;

	_computeShader.useProgram();

	const int PARAMETER_NUMBER = _parameters.size();
	float* strengthFactor = new float[PARAMETER_NUMBER];
	float* scale = new float[PARAMETER_NUMBER];
	float* offsetX = new float[PARAMETER_NUMBER];
	float* offsetY = new float[PARAMETER_NUMBER];
	int* type = new int[PARAMETER_NUMBER];
	float* baseValue = new float[PARAMETER_NUMBER];

	for (int i = 0; i < (int)_parameters.size(); i++)
	{
		std::vector<float>& param = _parameters[i].parameters;

		strengthFactor[i] = param.size() > 0 ? param[0] : 0;
		scale[i] = param.size() > 1 ? param[1] : 0;
		offsetX[i] = param.size() > 2 ? param[2] : 0;
		offsetY[i] = param.size() > 3 ? param[3] : 0;
		type[i] = (int)_parameters[i].type;

		float timeMultiplier = param.size() > 4 ? param[4] : 0;
		if (param.size() > 6 && param[6] == 1.0) // Moving scale
		{
			parameterNeedTime = true;
			scale[i] = sin(glfwGetTime() * timeMultiplier) * scale[i];
		}
		baseValue[i] = param.size() > 5 ? param[5] : 0;
	}

	const GLuint programId = _computeShader.getProgramID();
	glUniform1fv(glGetUniformLocation(programId, "strengthFactor"), PARAMETER_NUMBER, strengthFactor);
	glUniform1fv(glGetUniformLocation(programId, "scale"), PARAMETER_NUMBER, scale);
	glUniform1fv(glGetUniformLocation(programId, "offsetX"), PARAMETER_NUMBER, offsetX);
	glUniform1fv(glGetUniformLocation(programId, "offsetY"), PARAMETER_NUMBER, offsetY);
	glUniform1f(glGetUniformLocation(programId, "time"), glfwGetTime());
	glUniform1iv(glGetUniformLocation(programId, "type"), PARAMETER_NUMBER, type);
	glUniform1fv(glGetUniformLocation(programId, "baseValue"), PARAMETER_NUMBER, baseValue);

	glDispatchCompute(ceil(SCREEN_DIMENSION.x/8),ceil(SCREEN_DIMENSION.y/4),1);
	glMemoryBarrier(GL_ALL_BARRIER_BITS);

	delete[] strengthFactor;
	delete[] scale;
	delete[] offsetX;
	delete[] offsetY;
	delete[] type;
	delete[] baseValue;

	if (!parameterNeedTime) // Need to write on the param texture each frame to apply modifications
		_needToApplyChanges = false;
}

void InputParameter::setParameterValue(const int& parameterIndex, const std::vector<float>& parameterValues)
{
	if (parameterIndex < 0 || parameterIndex >= (int)_parameters.size())
	{
		std::cerr << "[InputParameter] invalid parameter index, aborting parameter modifications" << std::endl;
		return;
	}

	_parameters[parameterIndex].parameters = parameterValues;
	_needToApplyChanges = true;
}

std::vector<float>& InputParameter::getParameterValue(const int& index)
{
	return _parameters.at(index).parameters;
}

std::vector<Parameter>& InputParameter::getParametersValue()
{
	return _parameters;
}

InputParameterType InputParameter::getParameterType(const int& parameterIndex)
{
	if (parameterIndex < 0 || parameterIndex >= (int)_parameters.size())
	{
		std::cerr << "[InputParameter] invalid parameter index, aborting parameter modifications" << std::endl;
		return Number;
	}

	return _parameters[parameterIndex].type;
}
