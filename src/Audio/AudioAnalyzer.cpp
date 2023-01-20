#include "AudioAnalyzer.hpp"

AudioAnalyzer::AudioAnalyzer(int samplingRate, int samplesNumber, int outputArraySize)
	: _samplingRate(samplingRate), _samples(samplesNumber), _outputArraySize(outputArraySize)
{
	_fftIn = std::vector<kiss_fft_cpx>(_samples);
	_fftOut = std::vector<kiss_fft_cpx>(_samples);
	_maxFrequency = 15000;
	_inputArray = std::vector<float>(_samples, 0.0f);
	_outputArray = std::vector<float>(_outputArraySize, 0.0f);
}

void AudioAnalyzer::analyzeSignal(std::vector<float>& audioData)
{
	if ((int)audioData.size() < _samples) return;

	_previousOutputArray = _outputArray;

	std::lock_guard<std::mutex> guard(_outputArrayMutex);
	applyWindowFunction(audioData);
	shiftAudioData(audioData);
	computeFFT(_inputArray);
	findPeakFrequencies();
	convertToLog10();

	//displayOutputArrayInTerminal();

	//float ARRAY[(const int)_outputArraySize];

	//for (int i = 0; i < _outputArraySize; i++)
	//{
	//	ARRAY[i] = 0;
	//	if (_outputArray[i] <= 0) continue;

	//	if (_previousOutputArray[i])
	//		ARRAY[i] = _outputArray[i] / _previousOutputArray[i];
	//	else
	//		ARRAY[i] = _outputArray[i];
	//}

	//system("clear");
	//const int HEIGHT = 5;
	//for (int i = HEIGHT; i >= 0; i--)
	//{
	//	for (int j = 0; j < _outputArraySize; j++)
	//	{

	//	float coeff = 1.1;
	//	if (_outputArray[j] <= 5) coeff = 10;
	//	if (_outputArray[j] <= 10) coeff = 7.5;
	//	if (_outputArray[j] <= 7.5) coeff = 4;
	//	if (_outputArray[j] <= 15) coeff = 2;
	//	else if (_outputArray[j] <= 20) coeff = 1.25;
	//	else if (_outputArray[j] <= 30) coeff = 1.15;

	//		if (i == 0) {std::cout << "-"; continue; }
	//		if (ARRAY[j] >= coeff)
	//			std::cout << "O";
	//		else
	//			std::cout << " ";
	//		//if ((_outputArray[j]*2) > i)
	//		//	std::cout << "O";
	//		//else
	//		//	std::cout << " ";
	//	}
	//	std::cout << std::endl;
	//}
}

void AudioAnalyzer::applyWindowFunction(std::vector<float>& audioData)
{
	// Applying Hanning function
	for (int i = 0; i < _samples; i++)
	{
		_inputArray[i] = (audioData[i] * (0.5 * (1 - cos(2 * M_PI * i / (_samples - 1)))));
	}
}

void AudioAnalyzer::shiftAudioData(std::vector<float>& audioData)
{
	audioData.erase(audioData.begin(), audioData.begin() + _samples / 2);
}

void AudioAnalyzer::computeFFT(std::vector<float>& audioData)
{
	// Create an instance of the KissFFT class for the specified size
	kiss_fft_cfg cfg = kiss_fft_alloc(_samples, 0, nullptr, nullptr);

	// Copy the data from the input array to the KissFFT input array
	for (int i = 0; i < _samples; i++) {
		_fftIn[i].r = audioData[i];
		_fftIn[i].i = 0;
	}

	// Compute the DFT
	kiss_fft(cfg, _fftIn.data(), _fftOut.data());

	kiss_fft_cleanup();
	free(cfg);
}

void AudioAnalyzer::findPeakFrequencies()
{
	double freqBin[(const int)_outputArraySize];
	double incr = (float)_maxFrequency / (float)_outputArraySize;
	for (int i = 0; i < _outputArraySize; i++)
		freqBin[i] = incr * i;

	for (int i = 0; i < _outputArraySize; i++) _outputArray[i] = 1.7e-308;

	const int HALF_SAMPLES = _samples / 2;
	for (int i = 0; i < HALF_SAMPLES; i++)
	{
		float magnitude = sqrt((_fftOut[i].i *_fftOut[i].i)+(_fftOut[i].r*_fftOut[i].r));
		float freq = i * (double)_samplingRate / (float)HALF_SAMPLES;

		for (int j = 0; j < _outputArraySize; j++)
		{
			if((freq>freqBin[j]) && (freq <=freqBin[j+1]))
				if (magnitude > _outputArray[j])
					_outputArray[j] = magnitude;
		}
	}
}

void AudioAnalyzer::convertToLog10()
{
	for (int i = 0; i < _outputArraySize; i++)
		_outputArray[i] = 10 * log10(_outputArray[i]);
}

const std::vector<float>& AudioAnalyzer::getFrequencies() const
{
	return _outputArray;
}

void AudioAnalyzer::displayOutputArrayInTerminal() const
{
	system("clear");
	const int HEIGHT = 50;
	for (int i = HEIGHT; i >= 0; i--)
	{
		for (int j = 0; j < _outputArraySize; j++)
		{
			if (i == 0) {std::cout << "-"; continue; }
			if ((_outputArray[j]*2) > i)
				std::cout << "O";
			else
				std::cout << " ";
		}
		std::cout << std::endl;
	}
}
