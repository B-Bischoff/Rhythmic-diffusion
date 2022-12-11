#pragma once

#include "../inc/include.hpp"
#include "Application.hpp"

struct SimulationProperties;

class UserInterface {
private:
	GLFWwindow& _window;
	const int WIN_WIDTH, WIN_HEIGHT, UI_WIDTH;
	SimulationProperties& _simulationProperties;

public:
	UserInterface(GLFWwindow& window, const int& winWidth, const int& winHeight, const int& uiWidth, SimulationProperties& simulationProperties);

	void createNewFrame();
	void update();
	void render();
	void shutdown();

};
