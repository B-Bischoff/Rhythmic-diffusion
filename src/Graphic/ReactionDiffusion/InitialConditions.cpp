#include "./InitialConditions.hpp"

InitialConditions::InitialConditions()
{
}

InitialConditions::InitialConditions(const std::string& computeShaderPath)
{
	changeShader(computeShaderPath);
}

void InitialConditions::changeShader(const std::string& computeShaderPath)
{
	try {
		_computeShader = ComputeShader(computeShaderPath);
	}
	catch (const std::invalid_argument& e)
	{
		std::cerr << "[InitialConditions] cannot find compute shader: " << computeShaderPath << std::endl;
	}
}

void InitialConditions::execShader(const glm::vec2& screenDimensions)
{
	if (_computeShader.getProgramID() == 0)
		return;

	_computeShader.useProgram();

	// Apply shader parameters
	_computeShader.setFloat("radius", _radius);
	_computeShader.setFloat("border", _bordersSize);
	_computeShader.setFloat("angle", _rotationAngle);

	glDispatchCompute(ceil(screenDimensions.x/8),ceil(screenDimensions.y/4),1);
	glMemoryBarrier(GL_ALL_BARRIER_BITS);
}

void InitialConditions::setRadius(const float& radius) { _radius = radius; }
void InitialConditions::setBordersSize(const float& borderSize) { _bordersSize = borderSize; }
void InitialConditions::setRotationAngle(const float& rotationAngle) { _rotationAngle = rotationAngle; }
