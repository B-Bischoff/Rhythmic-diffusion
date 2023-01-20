#pragma once

#include "../../inc/include.hpp"
#include "./AudioAnalyzer.hpp"

class AudioPlayer {
private:

	// Stk and RtAudio members
	stk::StkFrames frames;
	stk::FileWvIn input;
	RtAudio dac;
	RtAudio::StreamParameters parameters;

	// vector containing audio data used by the frequency analyzer
	std::vector<float> _audioSamples;
	std::string _wavFileName;
	std::thread _audioPlayingThread;


	double _frameRead;
	bool _isPlaying;
	bool _songFinished;

	double _volume; // Percentage

	void startPlaying();

public:
	AudioPlayer();
	~AudioPlayer();

	int playWavFile(const std::string& wavFileName);
	int stopPlaying();
	void togglePause();

	void setTimestamp(const float& timestamp);
	void setVolume(const double& newVolume);
	void setAudioAnalyzer(AudioAnalyzer* audioAnalyzer);
	std::vector<float>& getAudioData();
	double getCurrentTimestamp() const;
	unsigned long getWavFileDuration();
	std::string getFileName() const;
	double getVolume() const;

	AudioAnalyzer* _audioAnalyzer; // TO PASS IN PRIVATE
};


// This tick() function handles sample computation only.
// It will be called automatically when the system needs a
// a new buffer of audio samples.
int tick( void *outputBuffer, void *inputBuffer, unsigned int nBufferFrames,
			double streamTime, RtAudioStreamStatus status, void *userData);

// This structure is used to pass in multiple parameters to
// the tick callback function
struct CallbackData {
	stk::FileWvIn* input;
	stk::StkFrames* frames;
	std::vector<float>* audioSamples;
	double* frameRead;
	bool* isPlaying;
	bool* songFinished;
	double* volume;
	AudioPlayer* audioPlayer;
};
