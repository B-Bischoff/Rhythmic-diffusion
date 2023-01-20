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
#include "./ReactionDiffusionSimulator.hpp"

class Application {
private:
	const glm::vec2 SCREEN_DIMENSION;
	GLFWwindow* _window;

	void loop();
	void printFps(float& deltaTime, float& lastFrame, int& fCounter);

public:
	Application(const int& width, const int& height);
	~Application();
};
