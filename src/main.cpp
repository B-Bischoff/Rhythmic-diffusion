#ifndef STB_IMAGE_IMPLEMENTATION
# define STB_IMAGE_IMPLEMENTATION
#endif


#include <signal.h>
#include <iostream>
#include <cstdlib>

#include <iostream>
#include <complex>
#include <vector>

#include "Application.hpp"

void kiss_main(std::vector<float>& datas, const int& samplesNb);
int stk_main(int argc, char* argv[]);

const double PI_ = 3.1415926535897932384626433832795;

int main(int argc, char* argv[])
{
	//Application application(1920, 1080);

	stk_main(argc, argv);

	return 0;
}

#include "../external/kissfft/kiss_fftr.h"
#include "../external/kissfft/kiss_fft.h"

void kiss_main(std::vector<float>& input, const int& samplesNb)
{
	const int FREQUENCY = 44100;

	// Set the size of the transform
	const int N = samplesNb;

	// Applying Hanning function
	for (int i = 0; i < N; i++)
		input[i] = input[i] * (0.5 * (1 - cos(2 * PI_ * i / (N - 1))));

	// Create an instance of the KissFFT class for the specified size
	kiss_fft_cfg cfg = kiss_fft_alloc(N, 0, nullptr, nullptr);

	// Allocate memory for the KissFFT input and output arrays
	std::vector<kiss_fft_cpx> in(N);
	std::vector<kiss_fft_cpx> out(N);

	// Copy the data from the input array to the KissFFT input array
	for (int i = 0; i < N; i++) {
		in[i].r = input[i];
		in[i].i = 0;
	}

	// Compute the DFT
	kiss_fft(cfg, in.data(), out.data());

	const int FREQ_NB = 600;
	double freq_bin[FREQ_NB]; //= {19.0, 140.0, 400.0, 2600.0, 5200.0, 22050 };
	double incr = 7000.0 / FREQ_NB;
	for (int i = 0; i < FREQ_NB; i++)
		freq_bin[i] = incr * i;

	const int COLUMNS = FREQ_NB;
	float peakMaxArray[COLUMNS];
	for (int i = 0; i < COLUMNS; i++) peakMaxArray[i] = 1.7e-308;

	for (int i = 0; i < N / 2; i++)
	{
		float magnitude = sqrt((out[i].i *out[i].i)+(out[i].r*out[i].r));
		float freq = i * (double)FREQUENCY / (float)(N/2.0);

		for (int j = 0; j < COLUMNS; j++)
		{
			if((freq>freq_bin[j]) && (freq <=freq_bin[j+1]))
				if (magnitude > peakMaxArray[j])
					peakMaxArray[j] = magnitude;
		}
	}
	for (int i = 0; i < FREQ_NB; i++)
		peakMaxArray[i] = 10*log10(peakMaxArray[i]);

	const int HEIGHT = 50;
	for (int i = HEIGHT; i >= 0; i--)
	{
		for (int j = 0; j < COLUMNS; j++)
		{
			if (i == 0) {std::cout << "-"; continue; }
			if ((peakMaxArray[j]*2) > i)
				std::cout << "O";
			else
				std::cout << " ";
		}
		std::cout << std::endl;
	}

	// Free the KissFFT resources
	kiss_fft_cleanup();
	free(cfg);
}

#include "vendor/stk/FileWvIn.h"
#include "vendor/stk/RtAudio.h"

using namespace stk;

bool done = false;
StkFrames frames;
static void finish(int ignore){ done = true; }


std::vector<float> datas;

void usage(void) {
	// Error function in case of incorrect command-line
	// argument specifications.
	std::cout << "\nuseage: play file sr <rate>\n";
	std::cout << "		where file = the file to play,\n";
	std::cout << "		where sr = sample rate,\n";
	std::cout << "		and rate = an optional playback rate.\n";
	std::cout << "							 (default = 1.0, can be negative)\n\n";
	exit( 0 );
}

// This tick() function handles sample computation only.	It will be
// called automatically when the system needs a new buffer of audio
// samples.
int tick( void *outputBuffer, void *inputBuffer, unsigned int nBufferFrames,
				double streamTime, RtAudioStreamStatus status, void *userData)
{
	FileWvIn *input = (FileWvIn *) userData;
	StkFloat *samples = (StkFloat *) outputBuffer;

	input->tick( frames );

	for ( unsigned int i=0; i<frames.size(); i++ )
	{
		*samples = frames[i] / 8;

		datas.push_back(frames[i] / 4);
		samples++;

		const int samplesNb = 4096;
		if (datas.size() >= samplesNb)
		{
			system("clear");
			kiss_main(datas, samplesNb);
			//datas.clear();
			datas.erase(datas.begin(), datas.begin() + samplesNb);
		}

		if ( input->channelsOut() == 1 ) {
			//std::cout << "mono" << std::endl;
			*samples++ = frames[i]; // play mono files in stereo
		}
	}

	if ( input->isFinished() ) {
		done = true;
		return 1;
	}
	else
		return 0;
}

int stk_main(int argc, char *argv[])
{
	// Minimal command-line checking.
	if ( argc < 3 || argc > 4 ) usage();

	// Set the global sample rate before creating class instances.
	Stk::setSampleRate( (StkFloat) atof( argv[2] ) );

	// Initialize our WvIn and RtAudio pointers.
	RtAudio dac;
	FileWvIn input;

	// Try to load the soundfile.
	try {
		input.openFile( argv[1] );
	}
	catch ( StkError & ) {
		exit( 1 );
	}

	// Set input read rate based on the default STK sample rate.
	double rate = 1.0;
	rate = input.getFileRate() / Stk::sampleRate();
	if ( argc == 4 ) rate *= atof( argv[3] );
	input.setRate( rate );

	input.ignoreSampleRateChange();

	// Find out how many channels we have.
	int channels = input.channelsOut();

	// Figure out how many bytes in an StkFloat and setup the RtAudio stream.
	RtAudio::StreamParameters parameters;
	parameters.deviceId = dac.getDefaultOutputDevice();
	parameters.nChannels = ( channels == 1 ) ? 2 : channels; //	Play mono files as stereo.
	RtAudioFormat format = ( sizeof(StkFloat) == 8 ) ? RTAUDIO_FLOAT64 : RTAUDIO_FLOAT32;
	unsigned int bufferFrames = RT_BUFFER_SIZE;
	try {
		dac.openStream( &parameters, NULL, format, (unsigned int)Stk::sampleRate(), &bufferFrames, &tick, (void *)&input );
	}
	catch ( RtAudioError &error ) {
		error.printMessage();
		goto cleanup;
	}

	// Install an interrupt handler function.
	(void) signal(SIGINT, finish);

	// Resize the StkFrames object appropriately.
	frames.resize( bufferFrames, channels );

	try {
		dac.startStream();
	}
	catch ( RtAudioError &error ) {
		error.printMessage();
		goto cleanup;
	}

	// Block waiting until callback signals done.
	while ( !done )
		Stk::sleep( 100 );

	// By returning a non-zero value in the callback above, the stream
	// is automatically stopped.	But we should still close it.
	try {
		dac.closeStream();
	}
	catch ( RtAudioError &error ) {
		error.printMessage();
	}

cleanup:
	return 0;
}
