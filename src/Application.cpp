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


	GLFWmonitor* monitor = glfwGetPrimaryMonitor();
	const GLFWvidmode* mode = glfwGetVideoMode(monitor);

	glfwWindowHint(GLFW_RED_BITS, mode->redBits);
	glfwWindowHint(GLFW_GREEN_BITS, mode->greenBits);
	glfwWindowHint(GLFW_BLUE_BITS, mode->blueBits);
	glfwWindowHint(GLFW_REFRESH_RATE, mode->refreshRate);
	_window = glfwCreateWindow(SCREEN_DIMENSION.x, SCREEN_DIMENSION.y, "Rhythmic-diffusion", monitor, NULL);
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

	// Application icon
	GLFWimage icons[1];
	std::string iconPath(PATH_PREFIX);
	iconPath += "icon.png";
	icons[0].pixels = stbi_load(iconPath.c_str(), &icons[0].width, &icons[0].height, 0, 4);
	if (icons[0].pixels == NULL)
		std::cout << "Failed to load icon" << std::endl;
	else
	{
		glfwSetWindowIcon(_window, 1, icons);
		stbi_image_free(icons[0].pixels);
	}

	loop();
}

Application::~Application()
{
}

void Application::loop()
{
	float previousTime = glfwGetTime();
	float fps = 0.0;
	float aimedFps = 200;

	AudioPlayer audioPlayer;
	AudioAnalyzer audioAnalyzer;
	audioPlayer.setAudioAnalyzer(&audioAnalyzer);

	ReactionDiffusionSimulator RDSimulator(_window, SCREEN_DIMENSION);

	Adapter adapter(RDSimulator, audioAnalyzer);
	Preset presetManager(RDSimulator, adapter);
	UserInterface ui(*_window, SCREEN_DIMENSION.x, SCREEN_DIMENSION.y, 550, RDSimulator, audioPlayer, audioAnalyzer, adapter, presetManager, aimedFps, fps);

	RDSimulator.setUIGradient(ui.getGradient());
	RDSimulator.setSimulationSpeed(1.0);
	RDSimulator.setParameterValue(0, std::vector<float>(1, 0.387));
	RDSimulator.setParameterValue(1, std::vector<float>(1, 0.276));
	RDSimulator.setParameterValue(2, std::vector<float>(1, 0.013));
	RDSimulator.setParameterValue(3, std::vector<float>(1, 0.038));
	RDSimulator.addInitialConditionsShape(InitialConditionsShape(Circle, 200, 10, 0, glm::vec2(0)));
	std::vector<glm::vec4> initGradient = { glm::vec4(0, 0, 0.15, 0), glm::vec4(1, 1, 1, 1) };
	RDSimulator.setPostProcessingGradient(initGradient, 3);

	while (!glfwWindowShouldClose(_window) && glfwGetKey(_window, GLFW_KEY_ESCAPE) != GLFW_PRESS)
	{
		float currentTime = glfwGetTime();

		if (currentTime - previousTime < 1.0 / aimedFps)
		{
			stk::Stk::sleep(1.0 / aimedFps);
			continue;
		}

		glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		ui.createNewFrame();

		adapter.update();
		ui.update();

		presetManager.updateAutomaticPresetSwitch();

		RDSimulator.processSimulation();

		RDSimulator.printRendering();
		ui.render();

		fps = 1.0 / (currentTime - previousTime);
		previousTime = currentTime;

		glfwSwapBuffers(_window);
		glfwPollEvents();
	}

	audioPlayer.stopPlaying();

	glfwTerminate();
}
