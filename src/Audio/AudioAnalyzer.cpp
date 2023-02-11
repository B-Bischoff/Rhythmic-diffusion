#include "AudioAnalyzer.hpp"
#include <iomanip>

AudioAnalyzer::AudioAnalyzer(int samplingRate, int samplesNumber, int outputArraySize)
	: _samplingRate(samplingRate), _samples(samplesNumber), _outputArraySize(outputArraySize)
{
	_fftIn = std::vector<kiss_fft_cpx>(_samples);
	_fftOut = std::vector<kiss_fft_cpx>(_samples);
	_maxFrequency = 11000;
	_inputArray = std::vector<float>(_samples, 0.0f);
	_outputArray = std::vector<float>(_outputArraySize, 0.0f);

	// Pre-calculating split index // CHANGE SPLIT INDEX NAME

	// fftOutSize can be less than _samples to exclude frequencies from the analysis
	// when fftOutSize = _samples, analyse range: [0 - 44100]Hz
	const int fftOutSize = _samples/5.5;
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

	// Convert raw audio in frequencies
	//std::lock_guard<std::mutex> guard(_outputArrayMutex);
	applyWindowFunction(audioData);
	shiftAudioData(audioData);
	computeFFT(_inputArray);
	//findPeakFrequencies();
	divideFFTOuputInSubbands();
	//convertToLog10();

	// Instrument / Melodic analyze
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

void AudioAnalyzer::findPeakFrequencies()
{
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
		//float magnitude = ((_fftOut[i].i *_fftOut[i].i)+(_fftOut[i].r*_fftOut[i].r));
		float freq = i * (double)_samplingRate / (float)HALF_SAMPLES;

		for (int j = 0; j < _outputArraySize; j++)
		{
			if((freq>freqBin[j]) && (freq <=freqBin[j+1]))
				if (magnitude > _outputArray[j])
					_outputArray[j] = magnitude;
		}
	}
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

void AudioAnalyzer::findLead()
{
	const int batchNumber = 43;
	const int bandNumber = _outputArraySize;

	// calculate instant energy
	float instantEnergy[bandNumber];
	for (int i = 0; i < bandNumber; i++)
		instantEnergy[i] = _outputArray[i];

	// calculate local average energy
	// CHANGE THIS TO MEMBER VARIABLE
	static std::vector<std::vector<float>> history(bandNumber, std::vector<float>(batchNumber, 0));

	float localAverageEnergy[bandNumber];
	for (int i = 0; i < bandNumber; i++)
		localAverageEnergy[i] = 0;

	for (int j = 0; j < bandNumber; j++)
	{
		for (int i = 0; i < batchNumber; i++)
			localAverageEnergy[j] += history[j][i];

		localAverageEnergy[j] /= batchNumber;
	}

	// calculate variance
	float variance[bandNumber];
	for (int i = 0; i < bandNumber; i++)
		variance[i] = 0;

	for (int j = 0; j < bandNumber; j++)
	{
		for (int i = 0; i < batchNumber; i++)
		{
			float temp = (history[j][i] - localAverageEnergy[j]);
			variance[j] += temp * temp;
		}
		variance[j] /= batchNumber;
	}

	// calculate sensitivity
	float sensitivity[bandNumber];
	for (int i = 0; i < bandNumber; i++)
	{
		float sensCoeff = -0.0025714;
		sensitivity[i] = (sensCoeff * variance[i]) + 1.5142857; // Linear degression
	}

	// grouping beats
	_groups.clear();
	//system("clear");

	for (int i = 0; i < bandNumber; i++)
	{
		bool beat = instantEnergy[i] > localAverageEnergy[i] * sensitivity[i];
		float delta = instantEnergy[i] - history[i][batchNumber - 1];
		float ratio = instantEnergy[i] / history[i][batchNumber - 1];

		if (beat && (ratio < 1.5 || instantEnergy[i] < 1.5)) // Noise filter
			beat = false;
		if (instantEnergy[i] - history[i][batchNumber-1] < 1)
			beat = false;

		//const std::string white = "\033[39m";
		//const std::string red = "\033[31m";
		//const std::string green = "\033[32m";
		//const std::string blue = "\033[34m";
		//const std::string orange = "\033[33m";


		// Group finding algo

		int grpIndex = -1;
		if (beat)
		{
			bool groupFound = false;
			const float groupThreshold = 5;

			for (int j = 0; j < _groups.size(); j++)
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
		//switch(grpIndex)
		//{
		//	case 0: color = red.c_str(); break;
		//	case 1: color = blue.c_str(); break;
		//	case 2: color = green.c_str(); break;
		//	case 3: color = orange.c_str(); break;
		//	default: color = white.c_str();
		//}

		//if (beat) color = red.c_str();
		//	std::cout << color;

		//std::cout << "[" << (i < 10 ? " " : "") << i << "]: " << (beat ? "0 " : "  ");
		//std::cout << std::fixed << std::setprecision(2) << (instantEnergy[i] - history[i][batchNumber-1]) << " ";
		//std::cout << std::fixed << std::setprecision(2) << instantEnergy[i] << " ";
		//std::cout << std::fixed << std::setprecision(2) << grpIndex << " ";
		//std::cout << std::fixed << std::setprecision(2) << (instantEnergy[i] / history[i][batchNumber-1]) << " ";
		//std::cout << std::fixed << std::setprecision(2) << sensitivity[i] << " ";
		//std::cout << std::fixed << std::setprecision(2) << variance[i] << " ";
		//std::cout << std::fixed << std::setprecision(2) << instantEnergy[i] << " ";
		//std::cout << std::fixed << std::setprecision(2) << localAverageEnergy[i] << " ";
		//std::cout << std::fixed << std::setprecision(2) << history[i][batchNumber-1] << " ";
		//std::cout << std::fixed << std::setprecision(2) << delta << " ";
		//if ((32 - i - 1) % 8 == 0)
		//	std::cout << std::endl;
	}

	//bool bass = false;
	//bool kick = false;
	//bool snare = false;

	//for (int i = 0; i < _groups.size(); i++)
	//{
		//std::cout << "group[" << i << "]" << " ";
		//std::cout << "meanIndex: " << _groups[i].getMeanIndex() << " | ";
		//std::cout << "meanDelta: " << _groups[i].getMeanDelta() << " | ";
		//std::cout << "bandNumber: " << _groups[i].getBandNumber() << " | ";
		//std::cout << std::endl;
		//if (_groups[i].getMeanIndex() < 7 && _groups[i].getMeanDelta() > 50)\
		//	bass = true;
	//}
	//std::cout << bass << std::endl;
	//std::cout << std::endl;

	//float globalVariance = 0;
	//for (int i = 0; i < bandNumber; i++)
	//{
	//	for (int j = 0; j < batchNumber; j++)
	//	{
	//		float temp = (history[i][j] - localAverageEnergy[i]);
	//		globalVariance += temp * temp;
	//	}
	//	globalVariance /= batchNumber;
	//}

	// update history
	for (int i = 0; i < bandNumber; i++)
	{
		history[i].erase(history[i].begin());
		history[i].push_back(instantEnergy[i]);
	}

	//std::cout << std::endl << "-------------------------------------" << std::endl;
	//std::cout << ((instantEnergy > localAverageEnergy*sensitivity) ? "O" : " ") << " - ";
}

bool AudioAnalyzer::isBass() const { return _isBass; };
bool AudioAnalyzer::isSnare() const { return _isSnare; };
bool AudioAnalyzer::isLead() const { return _isLead; };
const int AudioAnalyzer::getOutputArraySize() const { return _outputArraySize; }
