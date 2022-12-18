#include "Application.hpp"

float deltaTime = 0.0f;
float lastFrame = 0.0f;

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
	_diffusionReactionShader = ComputeShader("src/shaders/computeShader.cs");
	_inputShader = ComputeShader("src/shaders/inputCircle.cs");
	_colorOutputShader = ComputeShader("src/shaders/display.cs");
	_diffusionRateAShader = ComputeShader("src/shaders/numberInput.cs");
	_diffusionRateBShader = ComputeShader("src/shaders/numberInput.cs");
	_feedRateShader = ComputeShader("src/shaders/numberInput.cs");
	_killRateShader = ComputeShader("src/shaders/voronoi.cs");
	//_killRateShader = ComputeShader("src/shaders/numberInput.cs");

	_shader = Shader("src/shaders/shader.vert", "src/shaders/shader.frag");

	_compute0Texture = Texture(SCREEN_DIMENSION.x, SCREEN_DIMENSION.y);
	_compute1Texture = Texture(SCREEN_DIMENSION.x, SCREEN_DIMENSION.y);
	_parametersTexture = Texture(SCREEN_DIMENSION.x, SCREEN_DIMENSION.y);
	_finalTexture = Texture(SCREEN_DIMENSION.x, SCREEN_DIMENSION.y);

	_plane = Object(positions, textureCoords, indices);

	memset(&_simulationProperties, 0, sizeof(SimulationProperties));

	UserInterface ui(*_window, SCREEN_DIMENSION.x, SCREEN_DIMENSION.y, 300, _simulationProperties);

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
	while (!glfwWindowShouldClose(_window) && glfwGetKey(_window, GLFW_KEY_ESCAPE) != GLFW_PRESS)
	{
		glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		ui.createNewFrame();
		ui.update();

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

	glfwTerminate();
}

void Application::processInputParameters()
{
	_parametersTexture.useTexture(0);

	_diffusionRateAShader.useProgram();
	_diffusionRateAShader.setFloat("value", _simulationProperties.diffusionRateA);
	_diffusionRateAShader.setVec4("channels", glm::vec4(1.0, 0.0, 0.0, 0.0));
	glDispatchCompute(ceil(SCREEN_DIMENSION.x/8),ceil(SCREEN_DIMENSION.y/4),1);
	glMemoryBarrier(GL_ALL_BARRIER_BITS);

	_diffusionRateBShader.useProgram();
	_diffusionRateBShader.setFloat("value", _simulationProperties.diffusionRateB);
	_diffusionRateBShader.setVec4("channels", glm::vec4(0.0, 1.0, 0.0, 0.0));
	glDispatchCompute(ceil(SCREEN_DIMENSION.x/8),ceil(SCREEN_DIMENSION.y/4),1);
	glMemoryBarrier(GL_ALL_BARRIER_BITS);

	_feedRateShader.useProgram();
	_feedRateShader.setFloat("value", _simulationProperties.feedRate);
	_feedRateShader.setVec4("channels", glm::vec4(0.0, 0.0, 1.0, 0.0));
	glDispatchCompute(ceil(SCREEN_DIMENSION.x/8),ceil(SCREEN_DIMENSION.y/4),1);
	glMemoryBarrier(GL_ALL_BARRIER_BITS);

	_killRateShader.useProgram();
	_killRateShader.setFloat("t", glfwGetTime());
	//_killRateShader.setFloat("value", _simulationProperties.killRate);
	_killRateShader.setVec4("channels", glm::vec4(0.0, 0.0, 0.0, 1.0));
	glDispatchCompute(ceil(SCREEN_DIMENSION.x/8),ceil(SCREEN_DIMENSION.y/4),1);
	glMemoryBarrier(GL_ALL_BARRIER_BITS);
}

void Application::processDiffusionReaction()
{
	_diffusionReactionShader.useProgram();

	glm::vec4 properties = {
		_simulationProperties.diffusionRateA,
		_simulationProperties.diffusionRateB,
		_simulationProperties.feedRate,
		_simulationProperties.killRate
	};

	_parametersTexture.useTexture(2);
	_diffusionReactionShader.setVec4("_Properties", properties);
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
	glDispatchCompute(ceil(SCREEN_DIMENSION.x/8),ceil(SCREEN_DIMENSION.y/4),1);
	glMemoryBarrier(GL_ALL_BARRIER_BITS);

	_shader.useProgram();
	_finalTexture.useTexture(0);

	_shader.setInt("screen", GL_TEXTURE0);
	_plane.render();
}
