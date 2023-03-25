#ifndef STB_IMAGE_IMPLEMENTATION
# define STB_IMAGE_IMPLEMENTATION
#endif

#include <signal.h>
#include <iostream>
#include <cstdlib>

#include <iostream>
#include <complex>
#include <vector>

#include "Application.hpp"

#include "Graphic/OpenGlHelper/ComputeShader.hpp"

float lerp(float thresholdA, float thresholdB, float value)
{
	return  (value - thresholdA) / (thresholdB - thresholdA);
}

int findColors(float thresholds[10], const int N, float t)
{
	// Clamp between 0 and 1
	int i = 0;
	while (t >= thresholds[i] && i < N - 1)
		i++;
	return i;
}

int main(void)
{
	Application application(1920, 1080);

	//std::vector<float> thresholds;
	//std::vector<float> colors;
	//thresholds.push_back(0.0);
	//thresholds.push_back(0.5);
	//thresholds.push_back(0.6);
	//thresholds.push_back(1.0);

	//int index = findColors(thresholds, 1.1);

	//float t = lerp(thresholds[index - 1], thresholds[index], 0.5)

	const float ex = 0.125;

	const int N = 4;
	float thresholds[10];
	for (int i = 0; i < 10; i++)
		thresholds[i] = 0;
	thresholds[0] = 0.0;
	thresholds[1] = 0.25;
	thresholds[2] = 0.75;
	thresholds[3] = 1.0;
	glm::vec4 colors[10];
	for (int i = 0; i < 10; i++)
		colors[i] = glm::vec4(0);
	colors[0] = glm::vec4(1,0,0,1);
	colors[1] = glm::vec4(0,1,0,1);
	colors[2] = glm::vec4(1,0,1,1);
	colors[3] = glm::vec4(0,0,1,1);
	int index = findColors(thresholds, N, ex);
	float t = lerp(thresholds[index - 1], thresholds[index], ex);

	glm::vec4 c = mix(colors[index - 1], colors[index], t);
	std::cout << c.x << std::endl;
	std::cout << c.y << std::endl;
	std::cout << c.z << std::endl;
	std::cout << c.w << std::endl;

	return 0;
}
