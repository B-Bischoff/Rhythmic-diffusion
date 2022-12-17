#pragma once

#include "../inc/include.hpp"
#include "./Shader.hpp"
#include "./ComputeShader.hpp"
#include "./Object.hpp"
#include "./Texture.hpp"
#include "./UserInterface.hpp"

struct SimulationProperties {
	float speed;
	float diffusionRateA;
	float diffusionRateB;
	float feedRate;
	float killRate;
	bool reset;
	glm::vec3 colorA;
	glm::vec3 colorB;
};

class Application {
private:
	const glm::vec2 SCREEN_DIMENSION;
	GLFWwindow* _window;

	void loop();

public:
	Application(const int& width, const int& height);
	~Application();
};
