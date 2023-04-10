#pragma once

#include "../../inc/include.hpp"
#include "../Audio/AudioPlayer.hpp"
#include "../Audio/AudioAnalyzer.hpp"

class AudioPlayer;
class AudioAnalyzer;

class AudioPlayerUI {
private:
	ImGui::FileBrowser& _fileBrowser;
	AudioPlayer& _audioPlayer;
	AudioAnalyzer& _audioAnalyzer;

public:
	AudioPlayerUI(ImGui::FileBrowser& fileBrowser, AudioPlayer& audioPlayer, AudioAnalyzer& audioAnalyzer);

	void print();
};
