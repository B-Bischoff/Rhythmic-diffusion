#pragma once

#include "../inc/include.hpp"
#include "./Shader.hpp"
#include "./ComputeShader.hpp"
#include "./Object.hpp"
#include "./Texture.hpp"
#include "./UserInterface.hpp"
#include "./InputParameter.hpp"
#include "./AudioPlayer.hpp"
#include "./AudioAnalyzer.hpp"

struct SimulationProperties {
	float speed;
	bool reset;
	glm::vec3 colorA;
	glm::vec3 colorB;
	glm::vec4 paramTextureVisu;
};

class Application {
private:
	const glm::vec2 SCREEN_DIMENSION;
	GLFWwindow* _window;

	ComputeShader _diffusionReactionShader;
	ComputeShader _inputShader;
	InputParameter _diffusionRateAShader, _diffusionRateBShader, _feedRateShader, _killRateShader;
	ComputeShader _colorOutputShader;

	Shader _shader;

	Texture _compute0Texture, _compute1Texture;
	Texture _parametersTexture;
	Texture _finalTexture;

	SimulationProperties _simulationProperties;

	Object _plane;

	AudioPlayer _audioPlayer;

	void loop();

	void processRendering(bool& init, int& currentTexture);
	void processInputParameters();
	void processDiffusionReaction();
	void printFinalTexture(const int& currentTexture);

public:
	Application(const int& width, const int& height);
	~Application();
};
