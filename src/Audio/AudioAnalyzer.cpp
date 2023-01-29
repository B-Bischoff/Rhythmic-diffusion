#include "AudioAnalyzer.hpp"

AudioAnalyzer::AudioAnalyzer(int samplingRate, int samplesNumber, int outputArraySize)
	: _samplingRate(samplingRate), _samples(samplesNumber), _outputArraySize(outputArraySize)
{
	_fftIn = std::vector<kiss_fft_cpx>(_samples);
	_fftOut = std::vector<kiss_fft_cpx>(_samples);
	_maxFrequency = 11000;
	_inputArray = std::vector<float>(_samples, 0.0f);
	_outputArray = std::vector<float>(_outputArraySize, 0.0f);
}

void AudioAnalyzer::analyzeSignal(std::vector<float>& audioData)
{
	if ((int)audioData.size() < _samples) return;

	// Convert raw audio in frequencies
	std::lock_guard<std::mutex> guard(_outputArrayMutex);
	applyWindowFunction(audioData);
	shiftAudioData(audioData);
	computeFFT(_inputArray);
	findPeakFrequencies();
	convertToLog10();

	// Instrument / Melodic analyze
	findBass();
	findSnare();
	findLead();

	//displayOutputArrayInTerminal();
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
	// Peak method
	double freqBin[(const int)_outputArraySize+1];
	double incr = (float)_maxFrequency / (float)_outputArraySize;
	for (int i = 0; i < _outputArraySize+1; i++)
		freqBin[i] = incr * i;

	for (int j = 0; j < _outputArraySize+1; j++)
		_outputArray[j] = 0;

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

	// Mean method
	//const float frequenciesPerBand = (float)_samples / 4.0 / _outputArraySize;

	//for (int i = 0; i < _outputArraySize; i++)
	//	_outputArray[i] = 0;

	//for (int i = 0; i < _samples / 4; i++)
	//{
	//	int index = i / frequenciesPerBand;
	//std::cout << "---------> " << index << std::endl;
	//	float magnitude = sqrt((_fftOut[i].i *_fftOut[i].i)+(_fftOut[i].r*_fftOut[i].r));
	//	_outputArray[index] += magnitude;
	//}

	//for (int i = 0; i < _outputArraySize; i++)
	//	_outputArray[i] /= frequenciesPerBand/8.0;
}

void AudioAnalyzer::convertToLog10()
{
	for (int i = 0; i < _outputArraySize; i++)
	{
		float log10value = 10 * log10(_outputArray[i]);
		if (log10value != INFINITY && log10value != -INFINITY)
			_outputArray[i] = log10value;
		if (_outputArray[i] < 0)
			_outputArray[i] = 0;
	}
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

void AudioAnalyzer::findBass()
{
	// calculate instant energy
	float instantEnergy = 0;
	// i starts to 1 to remove bass from analysis
	for (int i = 0; i < _outputArraySize; i++)
		instantEnergy += _outputArray[i];
	float bassEnergy = _outputArray[0];

	// calculate local average energy
	static std::vector<float> history(43, 0);
	float localAverageEnergy = 0;
	for (int i = 0; i < 43; i++)
		localAverageEnergy += history[i];
	localAverageEnergy /= 43;

	// calculate variance
	float variance = 0;
	for (int i = 0; i < 43; i++)
	{
		float temp = (history[i] - localAverageEnergy);
		variance += temp * temp;
	}
	variance /= 43;

	// calculate sensitivity
	float sensitivity = (-0.0025714 * variance) + 1.5142857; // Linear degression
	if (sensitivity < 0.9f) sensitivity = 0.9f;

	// update history
	history.erase(history.begin());
	history.push_back(instantEnergy);

	_isBass = (bassEnergy > localAverageEnergy * sensitivity);

	//std::cout << ((instantEnergy > localAverageEnergy*((sensitivity > 0.9f) ? sensitivity : 0.9f)) ? "O" : " ") << " - ";
	//std::cout << "instant energy: " << instantEnergy << " ";
	//std::cout << "local energy: " << localAverageEnergy << " ";
	//std::cout << "sensitivy: " << sensitivity << " ";
	//std::cout << std::endl;
}

void AudioAnalyzer::findSnare()
{
	const float threshold = 9.5; // To find automatically

	_isSnare = false;

	float mean = 0.0f;
	// i starts to 1 to skip bass
	for (int i = 1; i < _outputArraySize; i++)
	{
		if (fabs(_outputArray[i]) < INFINITY)
			mean += _outputArray[i];
	}
	mean /= (_outputArraySize - 1);

	if (mean > threshold)
		_isSnare = true;
}

void AudioAnalyzer::findLead()
{
	const int batchNumber = 43;

	// calculate instant energy
	float instantEnergy = 0;
	// i starts to 1 to remove bass from analysis
	for (int i = 1; i < _outputArraySize; i++)
		instantEnergy += _outputArray[i];

	// calculate local average energy
	static std::vector<float> history(batchNumber, 0);
	float localAverageEnergy = 0;
	for (int i = 0; i < batchNumber; i++)
		localAverageEnergy += history[i];
	localAverageEnergy /= batchNumber;

	// calculate variance
	float variance = 0;
	for (int i = 0; i < batchNumber; i++)
	{
		float temp = (history[i] - localAverageEnergy);
		variance += temp * temp;
	}
	variance /= batchNumber;

	// calculate sensitivity
	float sensitivity = (-0.0025714 * variance) + 1.5142857; // Linear degression
	if (sensitivity < 1.4f) sensitivity = 1.4f;

	// update history
	history.erase(history.begin());
	history.push_back(instantEnergy);

	_isLead = (instantEnergy > localAverageEnergy * sensitivity);

	std::cout << ((instantEnergy > localAverageEnergy*sensitivity) ? "O" : " ") << " - ";
	std::cout << "instant energy: " << instantEnergy << " ";
	std::cout << "local energy: " << localAverageEnergy << " ";
	std::cout << "sensitivy: " << sensitivity << " ";
	std::cout << std::endl;
}

bool AudioAnalyzer::isBass() const { return _isBass; };
bool AudioAnalyzer::isSnare() const { return _isSnare; };
bool AudioAnalyzer::isLead() const { return _isLead; };
const int AudioAnalyzer::getOutputArraySize() const { return _outputArraySize; }
