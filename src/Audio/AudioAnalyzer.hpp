#pragma once

#include "../../inc/include.hpp"

struct FrequencyData {
	const float magnitude;
	const int index;
};

struct SoundGroup {
	int bandNumber = 0;
	float mean = 0;
	int meanIndex = 0;
	SoundGroup()
		: bandNumber(0), mean(0)
	{}
	SoundGroup(int newBand, float newMean, int newMeanIndex)
		: bandNumber(newBand), mean(newMean), meanIndex(newMeanIndex)
	{}
};

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

	bool _isBass, _isSnare, _isLead;

	void applyWindowFunction(std::vector<float>& audioData);
	void computeFFT(std::vector<float>& audioData);
	void shiftAudioData(std::vector<float>& audioData);
	void findPeakFrequencies();
	void divideFFTOuputInSubbands();
	void convertToLog10();

	void findLead();

	void displayOutputArrayInTerminal() const;

public:
	std::mutex _outputArrayMutex; // MOVE IN PRIVATE
	AudioAnalyzer(int samplingRate = 44100,
				int samplesNumber = 2048,
				int outputArraySize = 64);

	void analyzeSignal(std::vector<float>& audioData);

	bool isBass() const;
	bool isSnare() const;
	bool isLead() const;
	const std::vector<float>& getFrequencies() const;
	const int getOutputArraySize() const;

	std::vector<SoundGroup> _groups; // MOVE IN PRIVATE
};
