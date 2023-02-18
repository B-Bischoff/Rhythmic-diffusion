#pragma once

#include "../../inc/include.hpp"
#include "SoundGroup.hpp"

class AudioAnalyzer {
private:
	std::vector<float> _outputArray;
	std::vector<float> _inputArray;

	std::vector<kiss_fft_cpx> _fftIn;
	std::vector<kiss_fft_cpx> _fftOut;

	int _samplingRate;
	int _samples;
	int _outputArraySize;
	int _maxFrequency;

	std::vector<int> _splitIndex;

	const int _historySize;
	std::vector<std::vector<float>> _instantEnergyHistory;

	std::vector<SoundGroup> _groups;

	void applyWindowFunction(std::vector<float>& audioData);
	void computeFFT(std::vector<float>& audioData);
	void shiftAudioData(std::vector<float>& audioData);
	void divideFFTOuputInSubbands();

	void findBeats();

	void displayOutputArrayInTerminal() const;

public:
	std::mutex _outputArrayMutex; // MOVE IN PRIVATE
	AudioAnalyzer(int samplingRate = 44100,
				int samplesNumber = 2048,
				int outputArraySize = 64);

	void analyzeSignal(std::vector<float>& audioData);

	const std::vector<float>& getFrequencies() const;
	const int getOutputArraySize() const;
	std::vector<SoundGroup>& getGroups();
};
