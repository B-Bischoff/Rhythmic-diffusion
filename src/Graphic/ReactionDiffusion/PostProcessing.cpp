#include "PostProcessing.hpp"

PostProcessing::PostProcessing()
{
	_UIGradient = nullptr;
	_smoothGradientUpdate = false;
}

PostProcessing::PostProcessing(Texture* texture, const std::string& computeShaderPath)
	: _outputTexture(texture)
{
	_computeShader = ComputeShader(computeShaderPath);

	_gradient.push_back(glm::vec4(0));
	_gradient.push_back(glm::vec4(1));
	_parametersTexturePreview = glm::vec4(0);
	_UIGradient = nullptr;
	_smoothGradientUpdate = false;
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

void PostProcessing::setGradient(const std::vector<glm::vec4>& gradient, const double& duration)
{
	if (duration <= 0) // No transition
	{
		_gradient = gradient;
		return;
	}

	_transitionBegining = std::chrono::system_clock::now();
	_smoothGradientUpdate = true;
	_oldGradient = _gradient;
	_newGradient = gradient;

	// Add colors if the new gradient has more colors that the current one
	const int sizeDiff = _newGradient.size() - _gradient.size();
	if (sizeDiff > 0)
	{
		_gradient.insert(_gradient.end(), sizeDiff, glm::vec4(0, 0, 0, 1));
		_oldGradient.insert(_oldGradient.end(), sizeDiff, glm::vec4(0, 0, 0, 1));
	}
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

void PostProcessing::smoothGradientUpdate()
{
	std::chrono::system_clock::time_point currentTime = std::chrono::system_clock::now();
	std::chrono::duration<double> elapsedTime = currentTime - _transitionBegining;

	for (int i = 0; i < (int)_oldGradient.size(); i++)
	{
		const glm::vec4& targetedColor = i <= (int)(_newGradient.size() - 1) ? _newGradient[i] : _newGradient[_newGradient.size() - 1];
		_gradient[i] = glm::mix(_oldGradient[i], targetedColor, elapsedTime.count() / 5.0);
	}

	if (elapsedTime.count() >= 5.0)
	{
		_smoothGradientUpdate = false;

		// Clean colors on the same position (happens if the old gradient has more colors than the new one)
		std::vector<glm::vec4>::iterator it = _gradient.begin();
		double previousPosition = std::floor(it->w * 100 + 0.5)/100; // Rounding to 2 decimals

		for (++it; it != _gradient.end(); it++)
		{
			const double w = std::floor(it->w * 100 + 0.5 / 100); // Rounding to 2 decimals
			if (w == previousPosition)
			{
				it = _gradient.erase(it);
				it--;
			}
			previousPosition = w;
		}
	}

	// Update gradient UI
	if (_UIGradient == nullptr)
		return;

	_UIGradient->getMarks().clear();
	for (int i = 0; i < (int)_gradient.size(); i++)
	{
		glm::vec4& gradientElem = _gradient[i];
		ImColor color(gradientElem.x, gradientElem.y, gradientElem.z, 1.0);
		_UIGradient->addMark(gradientElem.w, color);
	}
}

void PostProcessing::setUIGradient(ImGradient& uiGradient)
{
	_UIGradient = &uiGradient;
}

bool PostProcessing::getSmoothGradientUpdate()
{
	return _smoothGradientUpdate;
}
