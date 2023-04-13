#include "ReactionDiffusionSimulator.hpp"

ReactionDiffusionSimulator::ReactionDiffusionSimulator(GLFWwindow* window, const glm::vec2& screenDimensions)
	: _window(window), _screenDimensions(screenDimensions)
{
	_init = true;
	_currentTexture = 0;
	_parameterTexturesPreview = glm::vec4(0);

	initPlane();
	initTextures();
	initShaders();
}

void ReactionDiffusionSimulator::initPlane()
{
	std::vector<float> positions = {
		-1.0f, -1.0f, 0.0f,
		-1.0f, 1.0f, 0.0f,
		1.0f, 1.0f, 0.0f,
		1.0f, -1.0f, 0.0f
	};
	std::vector<float> textureCoords = {
		0.0f, 0.0f,
		0.0f, 1.0f,
		1.0f, 1.0f,
		1.0f, 0.0f
	};
	std::vector<unsigned int> indices = {
		0, 2, 1,
		0, 3, 2
	};

	_plane = Object(positions, textureCoords, indices);
}

void ReactionDiffusionSimulator::initTextures()
{
	_compute0Texture = Texture(_screenDimensions.x, _screenDimensions.y);
	_compute1Texture = Texture(_screenDimensions.x, _screenDimensions.y);
	_parametersTexture = Texture(_screenDimensions.x, _screenDimensions.y);
	_finalTexture = Texture(_screenDimensions.x, _screenDimensions.y);
}

void ReactionDiffusionSimulator::initShaders()
{
	try {
		_shader = Shader("src/shaders/shader.vert", "src/shaders/shader.frag");
	}
	catch (const std::invalid_argument& e)
	{
		std::cerr << "[ReactionDiffusionSimulator] cannot find plane shader" << std::endl;
		exit (1);
	}

	_diffusionReactionShader = ComputeShader("src/shaders/reactionDiffusion/grayScott.comp");

	std::vector<std::string> initialConditionsShadersfiles {
		"src/shaders/initialConditions/glslSpec.comp", // Specs must be the first compiled file
		"src/shaders/initialConditions/triangle.comp",
		"src/shaders/initialConditions/hexagon.comp",
		"src/shaders/initialConditions/circle.comp",
		"src/shaders/initialConditions/square.comp",
		"src/shaders/initialConditions/rectangle.comp",
		"src/shaders/initialConditions/initialConditionsMain.comp", // Main must be the last compiled file
	};
	_initialConditions = InitialConditions(initialConditionsShadersfiles);

	_colorOutputShader = ComputeShader("src/shaders/display.cs");

	std::vector<std::string> inputParametersShadersfiles {
		"src/shaders/parameters/glslParametersSpec.comp", // Specs must be the first compiled file
		"src/shaders/parameters/number.comp",
		"src/shaders/parameters/perlin.comp",
		"src/shaders/parameters/voronoi.comp",
		"src/shaders/parameters/parametersMain.comp", // Main must be the last compiled file
	};
	_inputParameter = InputParameter(&_parametersTexture, inputParametersShadersfiles);

	_postProcessing = PostProcessing(&_finalTexture, "src/shaders/display.cs");
}

void ReactionDiffusionSimulator::processSimulation()
{
	processInputParameters();

	_compute0Texture.useTexture(_currentTexture); // 0 = input texture | 1 = output texture
	_compute1Texture.useTexture(!_currentTexture);

	processInitialConditions(false);

	processDiffusionReaction();
	applyPostProcessing();

	_currentTexture = !_currentTexture; // Swap previous and current texture
}

void ReactionDiffusionSimulator::processInitialConditions(const bool initOnce)
{
	if (!_init)
		return;

	_initialConditions.execShader(_screenDimensions);

	if (initOnce)
		_init = false;
}

void ReactionDiffusionSimulator::processInputParameters()
{
	_parametersTexture.useTexture(0);

	_inputParameter.execShader(_screenDimensions);
}

void ReactionDiffusionSimulator::processDiffusionReaction()
{
	_diffusionReactionShader.useProgram();

	_parametersTexture.useTexture(2);
	_diffusionReactionShader.setFloat("_ReactionSpeed", _simulationSpeed);
	glDispatchCompute(ceil(_screenDimensions.x/8),ceil(_screenDimensions.y/4),1);
	glMemoryBarrier(GL_ALL_BARRIER_BITS);
}

void ReactionDiffusionSimulator::applyPostProcessing()
{
	if (_postProcessing.getSmoothGradientUpdate())
		_postProcessing.smoothGradientUpdate();

	if (_currentTexture == 1)
		_compute1Texture.useTexture(0);
	else
		_compute0Texture.useTexture(0);
	_finalTexture.useTexture(1);

	_postProcessing.execShader(_screenDimensions);
}

void ReactionDiffusionSimulator::printRendering()
{
	_shader.useProgram();
	_finalTexture.useTexture(0);
	_shader.setInt("screen", GL_TEXTURE0);
	_plane.render();
}

// --------------------- Global settings ---------------------

void ReactionDiffusionSimulator::resetSimulation()
{
	glClearTexImage(_compute0Texture.getTextureID(), 0, GL_RGBA, GL_FLOAT, 0);
	glClearTexImage(_compute1Texture.getTextureID(), 0, GL_RGBA, GL_FLOAT, 0);
	_init = true;
}

void ReactionDiffusionSimulator::setSimulationSpeed(const float& speed) { _simulationSpeed = speed; }

// --------------------- Initial conditions methods ---------------------

void ReactionDiffusionSimulator::addInitialConditionsShape(InitialConditionsShape shape)
{
	_initialConditions.addShape(shape);
}

std::vector<InitialConditionsShape>& ReactionDiffusionSimulator::getInitialConditionsShapes()
{
	return _initialConditions.getShapes();
}

void ReactionDiffusionSimulator::clearInitialConditionsShapes()
{
	_initialConditions.clearShapes();
}

void ReactionDiffusionSimulator::removeInitialConditionsShape(const int& index)
{
	_initialConditions.removeShape(index);
}

// --------------------- Reaction diffusion parameters methods ---------------------


void ReactionDiffusionSimulator::setParameterValue(const int& parameterIndex, const std::vector<float>& parameterValues)
{
	_inputParameter.setParameterValue(parameterIndex, parameterValues);
}

const std::vector<float>& ReactionDiffusionSimulator::getParameterValue(const int& parameterIndex)
{
	return _inputParameter.getParameterValue(parameterIndex);
}

const std::vector<Parameter>& ReactionDiffusionSimulator::getParametersValue()
{
	return _inputParameter.getParametersValue();
}

void ReactionDiffusionSimulator::setParameterType(const int& parameterIndex, const InputParameterType& type)
{
	_inputParameter.changeType(parameterIndex, type);
}

InputParameterType ReactionDiffusionSimulator::getParameterType(const int& parameterIndex)
{
	return _inputParameter.getParameterType(parameterIndex);
}

// --------------------- Post processing methods ---------------------

void ReactionDiffusionSimulator::setPostProcessingGradient(const std::vector<glm::vec4>& gradient, const double& duration)
{
	_postProcessing.setGradient(gradient, duration);
}

const std::vector<glm::vec4>& ReactionDiffusionSimulator::getPostProcessingGradient() const
{
	return _postProcessing.getGradient();
}

void ReactionDiffusionSimulator::setParameterPreview(const int& parameterIndex, const bool& value)
{
	_postProcessing.setParameterPreview(parameterIndex, value);
}

bool ReactionDiffusionSimulator::getParameterPreview(const int& parameterIndex) const
{
	return _postProcessing.getParameterPreview(parameterIndex);
}

void ReactionDiffusionSimulator::setUIGradient(ImGradient& uiGradient)
{
	_postProcessing.setUIGradient(uiGradient);
}
