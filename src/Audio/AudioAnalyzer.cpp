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

	// Pre-calculating split index // CHANGE SPLIT INDEX NAME

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
}

void AudioAnalyzer::analyzeSignal(std::vector<float>& audioData)
{
	if ((int)audioData.size() < _samples) return;

	//std::lock_guard<std::mutex> guard(_outputArrayMutex);
	applyWindowFunction(audioData);
	shiftAudioData(audioData);
	computeFFT(_inputArray);
	divideFFTOuputInSubbands();

	findBeats();

	//displayOutputArrayInTerminal();
}

void AudioAnalyzer::applyWindowFunction(std::vector<float>& audioData)
{
	// Applying Hanning function
	for (int i = 0; i < _samples; i++)
	{
		_inputArray[i] = (audioData[i] * (0.5 * (1 - cos(2 * 3.1415 * i / (_samples - 1)))));
	}
}

void AudioAnalyzer::shiftAudioData(std::vector<float>& audioData)
{
	audioData.erase(audioData.begin(), audioData.begin() + _samples / 2);
	//audioData.clear();
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
		//_outputArray[i] *= (sampleNumber * 0.05);
	}
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

void AudioAnalyzer::findBeats()
{
	const int bandNumber = _outputArraySize;

	// calculate instant energy
	float* instantEnergy = new float[bandNumber];
	for (int i = 0; i < bandNumber; i++)
		instantEnergy[i] = _outputArray[i];

	// calculate local average energy
	float* localAverageEnergy = new float[bandNumber];
	for (int i = 0; i < bandNumber; i++)
		localAverageEnergy[i] = 0;

	for (int j = 0; j < bandNumber; j++)
	{
		for (int i = 0; i < _historySize; i++)
			localAverageEnergy[j] += _instantEnergyHistory[j][i];

		localAverageEnergy[j] /= _historySize;
	}

	// calculate variance
	float* variance = new float[bandNumber];
	for (int i = 0; i < bandNumber; i++)
		variance[i] = 0;

	for (int j = 0; j < bandNumber; j++)
	{
		for (int i = 0; i < _historySize; i++)
		{
			float temp = (_instantEnergyHistory[j][i] - localAverageEnergy[j]);
			variance[j] += temp * temp;
		}
		variance[j] /= _historySize;
	}

	// calculate sensitivity
	float* sensitivity = new float[bandNumber];
	for (int i = 0; i < bandNumber; i++)
	{
		float sensCoeff = -0.0025714;
		sensitivity[i] = (sensCoeff * variance[i]) + 1.5142857; // Linear degression
	}

	// Noise filter
	for (int i = 0; i < bandNumber; i++)
	{
		bool beat = instantEnergy[i] > localAverageEnergy[i] * sensitivity[i];
		float delta = instantEnergy[i] - _instantEnergyHistory[i][_historySize - 1];
		float ratio = instantEnergy[i] / _instantEnergyHistory[i][_historySize - 1];

		// Noise filters
		if (delta < 0)
			beat = false;
		if (beat && (ratio < 1.5 || instantEnergy[i] < 1.5)) // Noise filter
			beat = false;
		if (instantEnergy[i] - _instantEnergyHistory[i][_historySize - 1] < 1)
			beat = false;
	}

	_groups.clear();
	for (int i = 0; i < bandNumber; i++)
	{
		bool beat = instantEnergy[i] > localAverageEnergy[i] * sensitivity[i];
		float delta = instantEnergy[i] - _instantEnergyHistory[i][_historySize - 1];
		float ratio = instantEnergy[i] / _instantEnergyHistory[i][_historySize - 1];

		const std::string white = "\033[39m";
		const std::string red = "\033[31m";
		//const std::string green = "\033[32m";
		//const std::string blue = "\033[34m";
		//const std::string orange = "\033[33m";


		// Group dispatch
		int grpIndex = -1;
		if (beat)
		{
			bool groupFound = false;

			for (int j = 0; j < (int)_groups.size(); j++)
			{
				// add current frequency to group
				if (_groups[j].canJoinGroup(instantEnergy[i], delta, i))
				{
					_groups[j].addToGroup(instantEnergy[i], delta, i);
					groupFound = true;
					grpIndex = j;
					break;
				}
			}
			if (!groupFound)
			{
				grpIndex = _groups.size();
				_groups.push_back(SoundGroup(1, instantEnergy[i], delta, i));
			}
		}

		//std::string color = white.c_str();
		//if (beat) color = red.c_str();
		//	std::cout << color;

		//std::cout << "[" << (i < 10 ? " " : "") << i << "]: " << (beat ? "0 " : "  ");
		//std::cout << std::fixed << std::setprecision(2) << ((instantEnergy[i] - history[i][_historySize-1])) << " ";
		//std::cout << std::fixed << std::setprecision(2) << instantEnergy[i] << " ";
		//std::cout << std::fixed << std::setprecision(2) << grpIndex << " ";
		//std::cout << std::fixed << std::setprecision(2) << (instantEnergy[i] / history[i][_historySize-1]) << " ";
		//std::cout << std::fixed << std::setprecision(2) << sensitivity[i] << " ";
		//std::cout << std::fixed << std::setprecision(2) << variance[i] << " ";
		//std::cout << std::fixed << std::setprecision(2) << instantEnergy[i] << " ";
		//std::cout << std::fixed << std::setprecision(2) << localAverageEnergy[i] << " ";
		//std::cout << std::fixed << std::setprecision(2) << history[i][_historySize-1] << " ";
		//std::cout << std::fixed << std::setprecision(2) << delta << " ";
		//if ((32 - i - 1) % 8 == 0)
		//	std::cout << std::endl;
	}

	// update history
	for (int i = 0; i < bandNumber; i++)
	{
		_instantEnergyHistory[i].erase(_instantEnergyHistory[i].begin());
		_instantEnergyHistory[i].push_back(instantEnergy[i]);
	}

	//std::cout << std::endl << "-------------------------------------" << std::endl;

	delete[] instantEnergy;
	delete[] localAverageEnergy;
	delete[] variance;
	delete[] sensitivity;
}

std::vector<SoundGroup>& AudioAnalyzer::getGroups() { return _groups; }
const int AudioAnalyzer::getOutputArraySize() const { return _outputArraySize; }
const std::vector<float>& AudioAnalyzer::getFrequencies() const { return _outputArray; }
