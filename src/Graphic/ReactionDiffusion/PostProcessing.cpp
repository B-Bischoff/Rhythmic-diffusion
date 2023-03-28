#include "PostProcessing.hpp"

PostProcessing::PostProcessing()
{
}

PostProcessing::PostProcessing(Texture* texture, const std::string& computeShaderPath)
	: _outputTexture(texture)
{
	_computeShader = ComputeShader(computeShaderPath);

	_gradient.push_back(glm::vec4(0));
	_gradient.push_back(glm::vec4(1));
	_parametersTexturePreview = glm::vec4(0);
}

void PostProcessing::execShader(const glm::vec2& SCREEN_DIMENSIONS)
{
	_computeShader.useProgram();

	_computeShader.setVec4("visualizeChannels", _parametersTexturePreview);
	_computeShader.setInt("colorNumber", (int)_gradient.size());
	for (int i = 0; i < (int)_gradient.size(); i++)
	{
		std::string uniformName = "gradient[" + std::to_string(i) + "]";
		GLint originsLocation = glGetUniformLocation(_computeShader.getProgramID(), uniformName.c_str());
		glUniform4f(originsLocation, _gradient[i].x, _gradient[i].y, _gradient[i].z, _gradient[i].w);
	}

	glDispatchCompute(ceil(SCREEN_DIMENSIONS.x/8),ceil(SCREEN_DIMENSIONS.y/4),1);
	glMemoryBarrier(GL_ALL_BARRIER_BITS);
}

void PostProcessing::setGradient(const std::vector<glm::vec4>& gradient)
{
	_gradient = gradient;
}

const std::vector<glm::vec4>& PostProcessing::getGradient() const
{
	return _gradient;
}


bool PostProcessing::getParameterPreview(const int& parameterIndex) const
{
	if (parameterIndex >= 0 && parameterIndex <= 3)
		return static_cast<bool>(_parametersTexturePreview[parameterIndex]);
	return false;
}

void PostProcessing::setParameterPreview(const int& parameterIndex, const bool& value)
{
	if (parameterIndex >= 0 && parameterIndex <= 3)
		_parametersTexturePreview[parameterIndex] = static_cast<float>(value);
}
