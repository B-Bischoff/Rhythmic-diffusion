#pragma once

#include "../inc/include.hpp"
#include "./Application.hpp"

struct SimulationProperties;
class InputParameter;
class AudioPlayer;
class AudioAnalyzer;

class UserInterface {
private:
	GLFWwindow& _window;
	const int WIN_WIDTH, WIN_HEIGHT, UI_WIDTH;
	SimulationProperties& _simulationProperties;
	InputParameter* _inputParameters[4];

	AudioPlayer& _audioPlayer;
	AudioAnalyzer& _audioAnalyzer;

	void printOptionsFields(const int& i);
	void printNumberTypeFields(const int& i, const float& min, const float& max);
	void printPerlinNoiseFields(const int& i);

	void printAudioPlayer();

	std::string getFieldNameFromIndex(const int& index) const;

public:
	UserInterface(GLFWwindow& window, const int& winWidth, const int& winHeight, const int& uiWidth, SimulationProperties& simulationProperties, AudioPlayer& audioPlayer, AudioAnalyzer& audioAnalyzer);

	void setInputParameters(InputParameter* p0, InputParameter* p1, InputParameter* p2, InputParameter* p3);

	void createNewFrame();
	void update(const float array[600]);
	void render();
	void shutdown();

};
