#pragma once

#include "../../inc/include.hpp"

#include "./Shader.hpp"
#include "./ComputeShader.hpp"
#include "./InputParameter.hpp"
#include "./Object.hpp"

class ReactionDiffusionSimulator {
private:
	GLFWwindow* _window;
	const glm::vec2& _screenDimensions;

	Texture _compute0Texture, _compute1Texture;
	Texture _parametersTexture;
	Texture _finalTexture;

	Shader _shader;
	ComputeShader _diffusionReactionShader;
	ComputeShader _inputShader;
	ComputeShader _colorOutputShader;

	Object _plane;

	bool _init;
	int _currentTexture;

	// Simulation properties
	float _simulationSpeed;
	glm::vec3 _colorA, _colorB;
	glm::vec4 _parameterTexturesPreview; // each component represents as a boolean if the input param should be printed

	// METHODS
	void initPlane();
	void initTextures();
	void initShaders();

	void processInputParameters();
	void processDiffusionReaction();
	void applyPostProcessing();

	InputParameter& getParameterFromIndex(const int& index);

public:
	InputParameter _diffusionRateAShader, _diffusionRateBShader, _feedRateShader, _killRateShader;

	ReactionDiffusionSimulator(GLFWwindow* window, const glm::vec2& screenDimensions);

	void processSimulation();
	void printRendering();

	void resetSimulation();
	void setSimulationSpeed(const float& speed);
	void setSimulationColorA(const glm::vec3& color);
	void setSimulationColorB(const glm::vec3& color);

	void setParameterPreview(const int& parameterIndex, const bool& preview);
	bool getParameterPreview(const int& parameterIndex) const;
	void setParameterValue(const int& parameterIndex, const std::vector<float>& parameterValues);
	const std::vector<float>& getParameterValue(const int& parameterIndex);
	void setParameterType(const int& parameterIndex, const InputParameterType& type);
	InputParameterType getParameterType(const int& parameterIndex);

	// Set parameter preview (parameter index, value -> bool)
	// Set parameter value (parameter index, vector containing values)
	// Set paramter type (parameter index, new type)
	// Get parameter type (parameter index)
};
