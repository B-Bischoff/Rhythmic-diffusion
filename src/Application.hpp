#pragma once

#include <iostream>
#include <istream>

#include "../inc/include.hpp"
#include "./Shader.hpp"
#include "./ComputeShader.hpp"
#include "./Object.hpp"
#include "./Texture.hpp"

class Application {
private:
	const glm::vec2 SCREEN_DIMENSION;
	GLFWwindow* _window;

	void loop();

public:
	Application(const int& width, const int& height);
	~Application();
};
