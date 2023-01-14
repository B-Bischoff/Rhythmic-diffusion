#include "Application.hpp"

float deltaTime = 0.0f;
float lastFrame = 0.0f;

using namespace stk;

bool done = false;
StkFrames frames;
static void finish(int ignore){ done = true; }
const double PI_ = 3.1415926535897932384626433832795;

float PEAK_MAX_ARRAY[600];


std::vector<float> datas;
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
	double incr = 15000.0 / FREQ_NB;
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
	{
		peakMaxArray[i] = 10*log10(peakMaxArray[i]);
		PEAK_MAX_ARRAY[i] = peakMaxArray[i];
	}

	// Display in terminal
	//const int HEIGHT = 50;
	//for (int i = HEIGHT; i >= 0; i--)
	//{
	//	for (int j = 0; j < COLUMNS; j++)
	//	{
	//		if (i == 0) {std::cout << "-"; continue; }
	//		if ((peakMaxArray[j]*2) > i)
	//			std::cout << "O";
	//		else
	//			std::cout << " ";
	//	}
	//	std::cout << std::endl;
	//}

	// Free the KissFFT resources
	kiss_fft_cleanup();
	free(cfg);
}



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
			//system("clear");
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


int stk_main(const std::string& fileName)
{
	// Set the global sample rate before creating class instances.
	Stk::setSampleRate((StkFloat)44100.0);

	// Initialize our WvIn and RtAudio pointers.
	RtAudio dac;
	FileWvIn input;

	// Try to load the soundfile.
	try {
		input.openFile(fileName.c_str());
	}
	catch ( StkError & ) {
		exit( 1 );
	}

	// Set input read rate based on the default STK sample rate.
	double rate = 1.0;
	rate = input.getFileRate() / Stk::sampleRate();
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

Application::Application(const int& width, const int& height)
	: SCREEN_DIMENSION(glm::vec2(width, height))
{
	// GLFW init
	if (!glfwInit())
	{
		std::cerr << "Failed to initialize GLFW." << std::endl;
		std::cin.get();
		exit(1);
	}

	// Window init
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

	_window = glfwCreateWindow(SCREEN_DIMENSION.x, SCREEN_DIMENSION.y, "TEST", NULL, NULL);
	if (_window == nullptr)
	{
		std::cerr << "Failed to initialize GLFW window." << std::endl;
		std::cin.get();
		glfwTerminate();
		exit(1);
	}
	glfwMakeContextCurrent(_window);
	glfwSwapInterval(0);

	// GLEW init
	glewExperimental = true; // Needed for core profile
	if (glewInit() != GLEW_OK)
	{
		std::cerr << "Failed to initialize GLEW." << std::endl;
		std::cin.get();
		glfwTerminate();
		exit(1);
	}

	//loop();
	//stk_main("../../Downloads/I-Hate-Models-Daydream-_ARTS020_.wav");

	// Threads crash test
	loop();
}

Application::~Application()
{
}

void Application::loop()
{
	std::vector<float> positions = {
		-1.0f, -1.0f, 0.0f,
		-1.0f, 1.0f, 0.0f,
		1.0f, 1.0f, 0.0f,
		1.0f, -1.0f, 0.0f
	};
	std::vector<float> textureCoords = {
		0.0f, 0.0f,
		0.0f, 1.0f,
		1.0f, 1.0f,
		1.0f, 0.0f
	};
	std::vector<unsigned int> indices = {
		0, 2, 1,
		0, 3, 2
	};

	_compute0Texture = Texture(SCREEN_DIMENSION.x, SCREEN_DIMENSION.y);
	_compute1Texture = Texture(SCREEN_DIMENSION.x, SCREEN_DIMENSION.y);
	_parametersTexture = Texture(SCREEN_DIMENSION.x, SCREEN_DIMENSION.y);
	_finalTexture = Texture(SCREEN_DIMENSION.x, SCREEN_DIMENSION.y);

	_diffusionReactionShader = ComputeShader("src/shaders/computeShader.cs");
	_inputShader = ComputeShader("src/shaders/inputCircle.cs");
	_colorOutputShader = ComputeShader("src/shaders/display.cs");
	_diffusionRateAShader = InputParameter(&_parametersTexture);
	_diffusionRateBShader = InputParameter(&_parametersTexture);
	_feedRateShader = InputParameter(&_parametersTexture);
	_killRateShader = InputParameter(&_parametersTexture);

	_shader = Shader("src/shaders/shader.vert", "src/shaders/shader.frag");


	_plane = Object(positions, textureCoords, indices);

	memset(&_simulationProperties, 0, sizeof(SimulationProperties));

	UserInterface ui(*_window, SCREEN_DIMENSION.x, SCREEN_DIMENSION.y, 550, _simulationProperties);
	ui.setInputParameters(&_diffusionRateAShader, &_diffusionRateBShader, &_feedRateShader, &_killRateShader);

	{ // -------------------- COMPUTE WORK GROUP INFO -----------------------
		int work_grp_cnt[3];
		glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 0, &work_grp_cnt[0]);
		glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 1, &work_grp_cnt[1]);
		glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 2, &work_grp_cnt[2]);
		std::cout << "Max work groups per compute shader" <<
			" x:" << work_grp_cnt[0] <<
			" y:" << work_grp_cnt[1] <<
			" z:" << work_grp_cnt[2] << "\n";

		int work_grp_size[3];
		glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 0, &work_grp_size[0]);
		glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 1, &work_grp_size[1]);
		glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 2, &work_grp_size[2]);
		std::cout << "Max work group sizes" <<
			" x:" << work_grp_size[0] <<
			" y:" << work_grp_size[1] <<
			" z:" << work_grp_size[2] << "\n";

		int work_grp_inv;
		glGetIntegerv(GL_MAX_COMPUTE_WORK_GROUP_INVOCATIONS, &work_grp_inv);
		std::cout << "Max invocations count per work group: " << work_grp_inv << "\n";
	}

	int fCounter = 0;
	int currentTexture = 0;
	bool init = true;
	std::thread audio(stk_main, std::string("../../Downloads/Sköne-Fin-de-ce-qui-est-relatif-à-la-Nature_-au-Temps_-à-la-Conscience-et-aux-Perspectives-_2h44_.wav"));
	while (!glfwWindowShouldClose(_window) && glfwGetKey(_window, GLFW_KEY_ESCAPE) != GLFW_PRESS)
	{
		glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		ui.createNewFrame();
		ui.update(PEAK_MAX_ARRAY);

		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;
		if(fCounter > 500) {
			std::cout << "FPS: " << 1 / deltaTime << std::endl;
			fCounter = 0;
		} else {
			fCounter++;
		}


		processInputParameters();

		_compute0Texture.useTexture(currentTexture); // 0 = input texture | 1 = output texture
		_compute1Texture.useTexture(!currentTexture);

		if (init)
		{
			_inputShader.useProgram();
			_inputShader.setFloat("time", glfwGetTime());
			glDispatchCompute(ceil(SCREEN_DIMENSION.x/8),ceil(SCREEN_DIMENSION.y/4),1);
			glMemoryBarrier(GL_ALL_BARRIER_BITS);
			//init = false;
		}

		// Clear images
		if (_simulationProperties.reset)
		{
			glClearTexImage(_compute0Texture.getTextureID(), 0, GL_RGBA, GL_FLOAT, 0);
			glClearTexImage(_compute1Texture.getTextureID(), 0, GL_RGBA, GL_FLOAT, 0);
			_simulationProperties.reset = false;
			init = true;
		}

		processDiffusionReaction();

		printFinalTexture(currentTexture);

		currentTexture = !currentTexture;

		ui.render();

		glfwSwapBuffers(_window);
		glfwPollEvents();
	}
	std::terminate();
	audio.join();

	glfwTerminate();
}

void Application::processInputParameters()
{
	_parametersTexture.useTexture(0);

	_diffusionRateAShader.execShader(glm::vec4(1, 0, 0, 0), SCREEN_DIMENSION);
	_diffusionRateBShader.execShader(glm::vec4(0, 1, 0, 0), SCREEN_DIMENSION);
	_feedRateShader.execShader(glm::vec4(0, 0, 1, 0), SCREEN_DIMENSION);
	_killRateShader.execShader(glm::vec4(0, 0, 0, 1), SCREEN_DIMENSION);
}

void Application::processDiffusionReaction()
{
	_diffusionReactionShader.useProgram();

	_parametersTexture.useTexture(2);
	_diffusionReactionShader.setFloat("_ReactionSpeed", _simulationProperties.speed);
	glDispatchCompute(ceil(SCREEN_DIMENSION.x/8),ceil(SCREEN_DIMENSION.y/4),1);
	glMemoryBarrier(GL_ALL_BARRIER_BITS);
}

void Application::printFinalTexture(const int& currentTexture)
{
	if (currentTexture == 1)
		_compute1Texture.useTexture(0);
	else
		_compute0Texture.useTexture(0);
	_finalTexture.useTexture(1);


	// Apply color computation to the image
	_colorOutputShader.useProgram();
	_colorOutputShader.setVec3("colorA", _simulationProperties.colorA);
	_colorOutputShader.setVec3("colorB", _simulationProperties.colorB);
	_colorOutputShader.setVec4("visualizeChannels", _simulationProperties.paramTextureVisu);
	glDispatchCompute(ceil(SCREEN_DIMENSION.x/8),ceil(SCREEN_DIMENSION.y/4),1);
	glMemoryBarrier(GL_ALL_BARRIER_BITS);

	_shader.useProgram();
	_finalTexture.useTexture(0);

	_shader.setInt("screen", GL_TEXTURE0);
	_plane.render();
}
