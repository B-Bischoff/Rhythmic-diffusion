#include "Application.hpp"

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

	_window = glfwCreateWindow(SCREEN_DIMENSION.x, SCREEN_DIMENSION.y, "Rhythmic-diffusion", NULL, NULL);
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

	loop();
}

Application::~Application()
{
}

void Application::loop()
{
	AudioPlayer audioPlayer;
	AudioAnalyzer audioAnalyzer;
	audioPlayer.setAudioAnalyzer(&audioAnalyzer);

	int fCounter = 0;


	ReactionDiffusionSimulator RDSimulator(_window, SCREEN_DIMENSION);

	Adapter adapter(RDSimulator, audioAnalyzer);

	Preset presetManager(RDSimulator, adapter);
	UserInterface ui(*_window, SCREEN_DIMENSION.x, SCREEN_DIMENSION.y, 550, RDSimulator, audioPlayer, audioAnalyzer, adapter, presetManager);
	presetManager.setUIGradient(ui.getGradient());
	float deltaTime = 0.0f;
	float lastFrame = 0.0f;

	RDSimulator.setSimulationSpeed(1.0);
	RDSimulator.setParameterValue(0, std::vector<float>(1, 0.387));
	RDSimulator.setParameterValue(1, std::vector<float>(1, 0.276));
	RDSimulator.setParameterValue(2, std::vector<float>(1, 0.013));
	RDSimulator.setParameterValue(3, std::vector<float>(1, 0.038));

	while (!glfwWindowShouldClose(_window) && glfwGetKey(_window, GLFW_KEY_ESCAPE) != GLFW_PRESS)
	{
		glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		printFps(deltaTime, lastFrame, fCounter);

		ui.createNewFrame();
		ui.update();

		adapter.update();

		RDSimulator.processSimulation();
		RDSimulator.printRendering();

		ui.render();

		glfwSwapBuffers(_window);
		glfwPollEvents();
	}

	audioPlayer.stopPlaying();

	glfwTerminate();
}

void Application::printFps(float& deltaTime, float& lastFrame, int& fCounter)
{
	float currentFrame = glfwGetTime();
	deltaTime = currentFrame - lastFrame;
	lastFrame = currentFrame;
	if(fCounter > 500)
	{
		std::cout << "FPS: " << 1 / deltaTime << std::endl;
		fCounter = 0;
	}
	else
		fCounter++;
}
