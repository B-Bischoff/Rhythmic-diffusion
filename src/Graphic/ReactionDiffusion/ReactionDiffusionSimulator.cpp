#include "ReactionDiffusionSimulator.hpp"

ReactionDiffusionSimulator::ReactionDiffusionSimulator(GLFWwindow* window, const glm::vec2& screenDimensions)
	: _window(window), _screenDimensions(screenDimensions)
{
	{ // -------------------- COMPUTE WORK GROUP INFO (NO CONCRETE USE) -----------------------
		int work_grp_cnt[3];
		glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 0, &work_grp_cnt[0]);
		glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 1, &work_grp_cnt[1]);
		glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 2, &work_grp_cnt[2]);
		std::cout << "Max work groups per compute shader" <<
			" x:" << work_grp_cnt[0] <<
			" y:" << work_grp_cnt[1] <<
			" z:" << work_grp_cnt[2] << "\n";

		int work_grp_size[3];
		glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 0, &work_grp_size[0]);
		glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 1, &work_grp_size[1]);
		glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 2, &work_grp_size[2]);
		std::cout << "Max work group sizes" <<
			" x:" << work_grp_size[0] <<
			" y:" << work_grp_size[1] <<
			" z:" << work_grp_size[2] << "\n";

		int work_grp_inv;
		glGetIntegerv(GL_MAX_COMPUTE_WORK_GROUP_INVOCATIONS, &work_grp_inv);
		std::cout << "Max invocations count per work group: " << work_grp_inv << "\n";
	}

	_init = true;
	_currentTexture = 0;
	_colorA = glm::vec3(0.05);
	_colorB = glm::vec3(0.9);
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
	//_diffusionReactionShader = ComputeShader("src/shaders/reactionDiffusion/mutipleNeighborhood.comp");
	//_diffusionReactionShader = ComputeShader("src/shaders/reactionDiffusion/brusselator.comp");

	_initialConditions = InitialConditions("src/shaders/initialConditions/circle.comp");

	_colorOutputShader = ComputeShader("src/shaders/display.cs");

	_diffusionRateAShader = InputParameter(&_parametersTexture);
	_diffusionRateBShader = InputParameter(&_parametersTexture);
	_feedRateShader = InputParameter(&_parametersTexture);
	_killRateShader = InputParameter(&_parametersTexture);
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

	// Thread input shaders ?
	_diffusionRateAShader.execShader(0, _screenDimensions);
	_diffusionRateBShader.execShader(1, _screenDimensions);
	_feedRateShader.execShader(2, _screenDimensions);
	_killRateShader.execShader(3, _screenDimensions);
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
	if (_currentTexture == 1)
		_compute1Texture.useTexture(0);
	else
		_compute0Texture.useTexture(0);
	_finalTexture.useTexture(1);

	// Apply color computation to the image
	_colorOutputShader.useProgram();
	_colorOutputShader.setVec3("colorA", _colorA);
	_colorOutputShader.setVec3("colorB", _colorB);
	_colorOutputShader.setVec4("visualizeChannels", _parameterTexturesPreview);
	glDispatchCompute(ceil(_screenDimensions.x/8),ceil(_screenDimensions.y/4),1);
	glMemoryBarrier(GL_ALL_BARRIER_BITS);
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

void ReactionDiffusionSimulator::setSimulationColorA(const glm::vec3& color) { _colorA = color; }

void ReactionDiffusionSimulator::setSimulationColorB(const glm::vec3& color) { _colorB = color; }

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

void ReactionDiffusionSimulator::setParameterPreview(const int& parameterIndex, const bool& preview)
{
	if (parameterIndex < 0 || parameterIndex > 3)
		return;

	_parameterTexturesPreview[parameterIndex] = static_cast<float>(preview);
}

bool ReactionDiffusionSimulator::getParameterPreview(const int& parameterIndex) const
{
	if (parameterIndex < 0 || parameterIndex > 3)
		return false;

	return static_cast<bool>(_parameterTexturesPreview[parameterIndex]);
}

void ReactionDiffusionSimulator::setParameterValue(const int& parameterIndex, const std::vector<float>& parameterValues)
{
	if (parameterIndex < 0 || parameterIndex > 3)
		return;

	InputParameter& inputParameter = getParameterFromIndex(parameterIndex);
	std::vector<float>& inputValues = inputParameter.getVectorParameters();
	inputValues = parameterValues;
}

const std::vector<float>& ReactionDiffusionSimulator::getParameterValue(const int& parameterIndex)
{
	InputParameter& inputParameter = getParameterFromIndex(parameterIndex);
	return inputParameter.getVectorParameters();
}

void ReactionDiffusionSimulator::setParameterType(const int& parameterIndex, const InputParameterType& type)
{
	if (parameterIndex < 0 || parameterIndex > 3)
		return;

	InputParameter& inputParameter = getParameterFromIndex(parameterIndex);
	inputParameter.changeType(type);
}

InputParameterType ReactionDiffusionSimulator::getParameterType(const int& parameterIndex)
{
	InputParameter& inputParameter = getParameterFromIndex(parameterIndex);
	return inputParameter.getType();
}

InputParameter& ReactionDiffusionSimulator::getParameterFromIndex(const int& index)
{
	switch(index)
	{
		case 0: return _diffusionRateAShader;
		case 1: return _diffusionRateBShader;
		case 2: return _feedRateShader;
		case 3: return _killRateShader;
		default: throw std::invalid_argument("[ReactionDiffusionSimulator]: bad index\n");
	}
}
