#include "AudioPlayer.hpp"

AudioPlayer::AudioPlayer()
{
	_wavFileName = std::string();
	_audioSamples.reserve(2048);

	// Set the global sample rate before creating class instances.
	stk::Stk::setSampleRate((stk::StkFloat)44100.0);
	input.setRate((stk::StkFloat)0.0);

	_volume = 10.0;
	_isPlaying = false;
	_streamOpened = false;
}

AudioPlayer::~AudioPlayer()
{
	if (isPlaying())
		stopPlaying();
}

int AudioPlayer::playWavFile(const std::string& wavFileName)
{
	_wavFileName = wavFileName;

	if (_wavFileName.find("\\") == std::string::npos)
		_wavFileNameWithoutPath = wavFileName;

	size_t lastSlash = _wavFileName.find_last_of("\\/");
	std::cout << lastSlash << std::endl;
	_wavFileNameWithoutPath = wavFileName.substr(lastSlash + 1, _wavFileName.length() - lastSlash);

	if (isPlaying())
		stopPlaying();
	_audioPlayingThread = std::thread(&AudioPlayer::startPlaying, this);

	return 0;
}

void AudioPlayer::startPlaying()
{
	try {
		input.openFile(_wavFileName.c_str());
	}
	catch (stk::StkError&) {
		std::cerr << "Audio file: " << _wavFileName << " not found or cannot be loaded." << std::endl;
		return;
	};

	_songFinished = false;
	_isPlaying = true;
	_frameRead = 0;

	input.ignoreSampleRateChange();

	// Find out how many channels there is
	int channels = input.channelsOut();

	parameters.deviceId = dac.getDefaultOutputDevice();
	parameters.nChannels = ( channels == 1 ) ? 2 : channels; //	Play mono files as stereo.
	RtAudioFormat format = ( sizeof(stk::StkFloat) == 8 ) ? RTAUDIO_FLOAT64 : RTAUDIO_FLOAT32;
	unsigned int bufferFrames = stk::RT_BUFFER_SIZE;
	CallbackData callbackData = { &input, &frames, &_audioSamples, &_frameRead, &_isPlaying, &_songFinished, &_volume ,this};

	try {
		dac.openStream(&parameters, NULL, format, (unsigned int)stk::Stk::sampleRate(), &bufferFrames, tick, (void *)&callbackData );
		stk::Stk::sleep(100);
		input.setRate((stk::StkFloat)0.0);
		_isPlaying = false;
	}
	catch ( RtAudioError &error ) {
		error.printMessage();
		return;
	}

	// Resize the StkFrames object appropriately.
	frames.resize( bufferFrames, channels );

	try {
		dac.startStream();
		_streamOpened = true;
	}
	catch ( RtAudioError &error ) {
		error.printMessage();
	}

	// Block waiting until callback signals done.
	while (!_songFinished)
	{
		stk::Stk::sleep(100);
	}

	// By returning a non-zero value in the callback above, the stream
	// is automatically stopped.	But we should still close it.
	try {
		dac.closeStream();
		_streamOpened = false;
	}
	catch ( RtAudioError &error ) {
		error.printMessage();
	}
}

int tick(void *outputBuffer, void *inputBuffer, unsigned int nBufferFrames,
				double streamTime, RtAudioStreamStatus status, void *userData)
{
	// Unpack userData
	CallbackData* callbackData = (CallbackData*)userData;

	if (*callbackData->isPlaying)
		*callbackData->frameRead += nBufferFrames;

	stk::FileWvIn* input = callbackData->input;
	stk::StkFloat* samples = (stk::StkFloat*) outputBuffer;
	stk::StkFrames* frames = callbackData->frames;
	std::vector<float>& audioSamples = *callbackData->audioSamples;
	AudioPlayer* audioPlayer = callbackData->audioPlayer;

	input->tick(*frames);

	for (unsigned int i=0; i<frames->size(); i++)
	{
		*samples = (*frames)[i] * (*callbackData->volume/100);
		if (audioPlayer->isPlaying())
			audioSamples.push_back((*frames)[i]);
		samples++;

		if (input->channelsOut() == 1) {
			*samples++ = (*frames)[i]; // play mono files in stereo
		}
	}

	if (audioPlayer->isPlaying())
		audioPlayer->_audioAnalyzer->analyzeSignal(audioSamples);

	if (input->isFinished()) {
		*callbackData->songFinished = true;
		return 1;
	}
	else
		return 0;
}

int AudioPlayer::stopPlaying()
{
	if (!input.isOpen())
		return 1;

	_songFinished = true;
	_isPlaying = false;
	_frameRead = 0;
	_audioPlayingThread.join();
	input.closeFile();
	input.reset();

	return 0;
}
void AudioPlayer::togglePause()
{
	_isPlaying = !_isPlaying;

	if (_isPlaying)
		input.setRate((stk::StkFloat)1.0);
	else
		input.setRate((stk::StkFloat)0.0);
}

void AudioPlayer::setTimestamp(const float& timestamp)
{
	input.addTime((stk::StkFloat)timestamp);
	_frameRead = _frameRead + timestamp;
}

void AudioPlayer::setAudioAnalyzer(AudioAnalyzer* audioAnalyzer)
{
	_audioAnalyzer = audioAnalyzer;
}

std::vector<float>& AudioPlayer::getAudioData()
{
	return _audioSamples;
}

double AudioPlayer::getCurrentTimestamp() const
{
	return _frameRead;
}

unsigned long AudioPlayer::getWavFileDuration()
{
	return input.getSize();
}

std::string AudioPlayer::getFileName() const
{
	return _wavFileName;
}

std::string AudioPlayer::getFileNameWithoutPath() const
{
	return _wavFileNameWithoutPath;
}

void AudioPlayer::setVolume(const double& newVolume)
{
	_volume = newVolume;
}

double AudioPlayer::getVolume() const
{
	return _volume;
}

bool AudioPlayer::isPlaying() const
{
	return _isPlaying;
}

bool AudioPlayer::getStreamOpened() const
{
	return _streamOpened;
}
