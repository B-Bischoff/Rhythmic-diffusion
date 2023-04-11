#pragma once

#include "../../../inc/include.hpp"

#include "./InputParameter.hpp"
#include "./InitialConditions.hpp"
#include "./PostProcessing.hpp"
#include "../OpenGlHelper/Shader.hpp"
#include "../OpenGlHelper/ComputeShader.hpp"
#include "../OpenGlHelper/Object.hpp"

class ReactionDiffusionSimulator {
private:
	GLFWwindow* _window;
	const glm::vec2& _screenDimensions;

	Texture _compute0Texture, _compute1Texture;
	Texture _parametersTexture;
	Texture _finalTexture;

	Shader _shader;
	ComputeShader _diffusionReactionShader;
	ComputeShader _colorOutputShader;

	InitialConditions _initialConditions;
	InputParameter _diffusionRateAShader, _diffusionRateBShader, _feedRateShader, _killRateShader;
	InputParameter _inputParameter;
	PostProcessing _postProcessing;

	Object _plane;

	bool _init;
	int _currentTexture;

	// Simulation properties
	float _simulationSpeed;
	glm::vec4 _parameterTexturesPreview; // each component represents as a boolean if the input param should be printed

	void initPlane();
	void initTextures();
	void initShaders();

	void processInitialConditions(const bool initOnce);
	void processInputParameters();
	void processDiffusionReaction();
	void applyPostProcessing();

	InputParameter& getParameterFromIndex(const int& index);

public:

	ReactionDiffusionSimulator(GLFWwindow* window, const glm::vec2& screenDimensions);

	void processSimulation();
	void printRendering();

	// Global settings
	void resetSimulation();
	void setSimulationSpeed(const float& speed);

	// Initial conditions methods
	void addInitialConditionsShape(InitialConditionsShape shape);
	std::vector<InitialConditionsShape>& getInitialConditionsShapes();
	void clearInitialConditionsShapes();
	void removeInitialConditionsShape(const int& index);

	// Reaction diffusion parameters methods
	void setParameterValue(const int& parameterIndex, const std::vector<float>& parameterValues);
	const std::vector<float>& getParameterValue(const int& parameterIndex);
	const std::vector<Parameter>& getParametersValue();
	void setParameterType(const int& parameterIndex, const InputParameterType& type);
	InputParameterType getParameterType(const int& parameterIndex);

	// Post processing methods
	void setPostProcessingGradient(const std::vector<glm::vec4>& gradient, const double& duration);
	const std::vector<glm::vec4>& getPostProcessingGradient() const;
	void setParameterPreview(const int& parameterIndex, const bool& value);
	bool getParameterPreview(const int& parameterIndex) const;
	void setUIGradient(ImGradient& uiGradient);
};
