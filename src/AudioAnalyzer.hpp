#pragma once

#include "../inc/include.hpp"

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

	void applyWindowFunction(std::vector<float>& audioData);
	void computeFFT(std::vector<float>& audioData);
	void shiftAudioData(std::vector<float>& audioData);
	void findPeakFrequencies();
	void convertToLog10();

public:
	std::mutex _outputArrayMutex; // MOVE IN PRIVATE
	AudioAnalyzer(int samplingRate = 44100,
				int samplesNumber = 4096,
				int outputArraySize = 600);

	void analyzeSignal(std::vector<float>& audioData);
	const std::vector<float>& getFrequencies() const;

	// ---- Methods ----
	// Compute audio signal (const vector<float>&)

	// ---- Parameters ----
	// Sampling rate (44100)
	// samples Nb (4096)
	// out array size
	// max frequency
};
