#pragma once

#include "../inc/include.hpp"
#include "./UserInterface.hpp"
#include "./Audio/AudioPlayer.hpp"
#include "./Audio/AudioAnalyzer.hpp"
#include "./Graphic/ReactionDiffusion/ReactionDiffusionSimulator.hpp"
#include "./Adapter.hpp"

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
