#include "AudioAnalyzer.hpp"
#include <iomanip>

AudioAnalyzer::AudioAnalyzer(int samplingRate, int samplesNumber, int outputArraySize)
	: _samplingRate(samplingRate), _samples(samplesNumber), _outputArraySize(outputArraySize), _historySize(43)
{
	_fftIn = std::vector<kiss_fft_cpx>(_samples);
	_fftOut = std::vector<kiss_fft_cpx>(_samples);
	_maxFrequency = 8000;
	_inputArray = std::vector<float>(_samples, 0.0f);
	_outputArray = std::vector<float>(_outputArraySize, 0.0f);

	_instantEnergyHistory = std::vector<std::vector<float>>(_outputArraySize, std::vector<float>(_historySize, 0));

	// Pre-calculating split index

	// fftOutSize can be less than _samples to exclude frequencies from the analysis
	// when fftOutSize = _samples, analyse range: [0 - 44100]Hz
	const int fftOutSize = _samples / (_samplingRate / _maxFrequency);
	const int firstSubbandWidth = 2;

	// Find slope coefficient
	float slopeCoeff = ((float)fftOutSize / (float)_outputArraySize) - firstSubbandWidth;
	slopeCoeff /= (_outputArraySize - 1);
	slopeCoeff *= 2;

	for (int i = 0; i < _outputArraySize; i++)
		_splitIndex.push_back(roundf(slopeCoeff * i + firstSubbandWidth));

	// Allocate buffers
	_instantEnergy = new float[_outputArraySize];
	_localAverageEnergy = new float[_outputArraySize];
	_variance = new float[_outputArraySize];
	_sensitivity = new float[_outputArraySize];
}

AudioAnalyzer::~AudioAnalyzer()
{
	delete[] _instantEnergy;
	delete[] _localAverageEnergy;
	delete[] _variance;
	delete[] _sensitivity;
}

void AudioAnalyzer::analyzeSignal(std::vector<float>& audioData)
{
	if ((int)audioData.size() < _samples) return;

	applyWindowFunction(audioData);
	shiftAudioData(audioData);
	computeFFT(_inputArray);
	divideFFTOuputInSubbands();

	findBeats();
}

void AudioAnalyzer::applyWindowFunction(std::vector<float>& audioData)
{
	// Applying Hanning function
	for (int i = 0; i < _samples; i++)
		_inputArray[i] = (audioData[i] * (0.5 * (1 - cos(2 * 3.1415 * i / (_samples - 1)))));
}

void AudioAnalyzer::shiftAudioData(std::vector<float>& audioData)
{
	// Shift the audio data half way through the vector
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

void AudioAnalyzer::divideFFTOuputInSubbands()
{
	// clear outputArray existing values
	for (int i = 0; i < _outputArraySize; i++)
		_outputArray[i] = 0;

	// Assign frequencies magnitude to outputArray
	int fftIndex = 0;
	for (int i = 0; i < _outputArraySize; i++)
	{
		int sampleNumber = _splitIndex[i];

		for (int j = 0; j < sampleNumber; j++)
		{
			float magnitude = sqrt((_fftOut[fftIndex].i *_fftOut[fftIndex].i)+(_fftOut[fftIndex].r*_fftOut[fftIndex].r));
			if (magnitude > _outputArray[i]) // Storing the highest magnitude only
				_outputArray[i] = magnitude;
			fftIndex++;
		}
	}
}

void AudioAnalyzer::findBeats()
{
	const int bandNumber = _outputArraySize;

	// calculate instant energy
	for (int i = 0; i < bandNumber; i++)
		_instantEnergy[i] = _outputArray[i];

	// calculate local average energy
	for (int i = 0; i < bandNumber; i++)
		_localAverageEnergy[i] = 0;

	for (int j = 0; j < bandNumber; j++)
	{
		for (int i = 0; i < _historySize; i++)
			_localAverageEnergy[j] += _instantEnergyHistory[j][i];

		_localAverageEnergy[j] /= _historySize;
	}

	// calculate variance
	for (int i = 0; i < bandNumber; i++)
		_variance[i] = 0;

	for (int j = 0; j < bandNumber; j++)
	{
		for (int i = 0; i < _historySize; i++)
		{
			float temp = (_instantEnergyHistory[j][i] - _localAverageEnergy[j]);
			_variance[j] += temp * temp;
		}
		_variance[j] /= _historySize;
	}

	// calculate sensitivity
	for (int i = 0; i < bandNumber; i++)
	{
		float sensCoeff = -0.0025714;
		_sensitivity[i] = (sensCoeff * _variance[i]) + 1.5142857; // Linear degression
	}

	// Noise filtering
	for (int i = 0; i < bandNumber; i++)
	{
		bool beat = _instantEnergy[i] > _localAverageEnergy[i] * _sensitivity[i];
		float delta = _instantEnergy[i] - _instantEnergyHistory[i][_historySize - 1];
		float ratio = _instantEnergy[i] / _instantEnergyHistory[i][_historySize - 1];

		if (delta < 0)
			beat = false;
		if (beat && (ratio < 1.5 || _instantEnergy[i] < 1.5))
			beat = false;
		if (_instantEnergy[i] - _instantEnergyHistory[i][_historySize - 1] < 1)
			beat = false;
	}

	_groups.clear();
	for (int i = 0; i < bandNumber; i++)
	{
		bool beat = _instantEnergy[i] > _localAverageEnergy[i] * _sensitivity[i];
		float delta = _instantEnergy[i] - _instantEnergyHistory[i][_historySize - 1];

		if (!beat)
			continue;

		// Group dispatch
		bool groupFound = false;

		for (int j = 0; j < (int)_groups.size(); j++)
		{
			// add current frequency to existing group
			if (_groups[j].canJoinGroup(_instantEnergy[i], delta, i))
			{
				_groups[j].addToGroup(_instantEnergy[i], delta, i);
				groupFound = true;
				break;
			}
		}
		if (!groupFound) // Create a new group
			_groups.push_back(SoundGroup(1, _instantEnergy[i], delta, i));
	}

	// update history
	for (int i = 0; i < bandNumber; i++)
	{
		_instantEnergyHistory[i].erase(_instantEnergyHistory[i].begin());
		_instantEnergyHistory[i].push_back(_instantEnergy[i]);
	}
}

std::vector<SoundGroup>& AudioAnalyzer::getGroups() { return _groups; }
const int AudioAnalyzer::getOutputArraySize() const { return _outputArraySize; }
const std::vector<float>& AudioAnalyzer::getFrequencies() const { return _outputArray; }
