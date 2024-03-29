#pragma once

#include "../../inc/include.hpp"
#include "../Application.hpp"

#include "AudioPlayerUI.hpp"
#include "HooksUI.hpp"
#include "InitialConditionsUI.hpp"
#include "PresetUI.hpp"
#include "RDOptionsUI.hpp"
#include "GradientUI.hpp"

class InputParameter;
class AudioPlayer;
class AudioAnalyzer;
class ReactionDiffusionSimulator;
class Adapter;
struct AdapterHook;
class Preset;

class UserInterface {
private:
	GLFWwindow& _window;
	const int WIN_WIDTH, WIN_HEIGHT, UI_WIDTH;
	ReactionDiffusionSimulator& _RDSimulator;
	ImGui::FileBrowser _fileBrowser;
	ImGradient _gradient;

	AudioPlayer& _audioPlayer;
	AudioAnalyzer& _audioAnalyzer;
	Adapter& _adapter;
	Preset& _presetManager;

	AudioPlayerUI _audioPlayerUI;
	HooksUI _hooksUI;
	InitialConditionsUI _initialConditionsUI;
	PresetUI _presetUI;
	RDOptionsUI _RDOptionsUI;
	GradientUI _gradientUI;

	float& _maxFps;
	const float& _fps;

	// Used to associate field with its min and max slider value (ex: "feedRate" - [0 - 0.06])
	std::map<std::string, glm::vec2> _slidersRanges;

public:
	UserInterface(GLFWwindow& window,
			const int& winWidth,
			const int& winHeight,
			const int& uiWidth,
			ReactionDiffusionSimulator& RDSimulator,
			AudioPlayer& audioPlayer,
			AudioAnalyzer& audioAnalyzer,
			Adapter& Adapter,
			Preset& presetManager,
			float& maxFps,
			const float& fps
			);
	~UserInterface();

	void createNewFrame();
	void update();
	void render();
	void shutdown();

	ImGradient& getGradient();
};
