#pragma once

#include "../inc/include.hpp"
#include "./Application.hpp"

class InputParameter;
class AudioPlayer;
class AudioAnalyzer;
class ReactionDiffusionSimulator;
class Adapter;
struct AdapterHook;

class UserInterface {
private:
	GLFWwindow& _window;
	const int WIN_WIDTH, WIN_HEIGHT, UI_WIDTH;
	ReactionDiffusionSimulator& _RDSimulator;

	AudioPlayer& _audioPlayer;
	AudioAnalyzer& _audioAnalyzer;
	Adapter& _adapter;

	void printOptionsFields(const int& i);
	void printNumberTypeFields(const int& i, const float& min, const float& max);
	void printPerlinNoiseFields(const int& i);

	void printAudioPlayer();
	void printAdapterHook();

	std::string getFieldNameFromIndex(const int& index) const;

public:
	UserInterface(GLFWwindow& window, const int& winWidth, const int& winHeight, const int& uiWidth, ReactionDiffusionSimulator& RDSimulator, AudioPlayer& audioPlayer, AudioAnalyzer& audioAnalyzer, Adapter& adpter);

	void createNewFrame();
	void update();
	void render();
	void shutdown();

};
