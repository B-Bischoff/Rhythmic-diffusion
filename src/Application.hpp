#pragma once

// OPENGL LIBRARY
#include <GL/glew.h>
//#include "../external/glew-2.2.0/include/GL/glew.h"
#include <GLFW/glfw3.h>
#include "glm/glm.hpp"
//#include "../external/glm-0.9.9.8/glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
//#include "../external/glm-0.9.9.8/glm/gtc/matrix_transform.hpp"

#include <iostream>
#include <istream>

#include "ShaderProgram.hpp"

class Application {
private:
	const glm::vec2 SCREEN_DIMENSION;
	GLFWwindow* _window;

	void loop();

public:
	Application(const int& width, const int& height);
	~Application();
};
