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

	ComputeShader computeShader("./src/shaders/computeShader.cs");
	ComputeShader displayShader("./src/shaders/display.cs");
	ComputeShader inputShader("./src/shaders/input1.cs");
	Shader shader("./src/shaders/shader.vert", "./src/shaders/shader.frag");
	Object plane(positions, textureCoords, indices);
	Texture texture0(SCREEN_DIMENSION.x, SCREEN_DIMENSION.y);
	Texture texture1(SCREEN_DIMENSION.x, SCREEN_DIMENSION.y);
	Texture outTexture(SCREEN_DIMENSION.x, SCREEN_DIMENSION.y);

	SimulationProperties simulationProperties;
	memset(&simulationProperties, 0, sizeof(simulationProperties));

	UserInterface ui(*_window, SCREEN_DIMENSION.x, SCREEN_DIMENSION.y, 300, simulationProperties);

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

		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;
		if(fCounter > 500) {
			std::cout << "FPS: " << 1 / deltaTime << std::endl;
			fCounter = 0;
		} else {
			fCounter++;
		}

		texture0.useTexture(currentTexture); // 0 = input texture | 1 = output texture
		texture1.useTexture(!currentTexture);

		if (init)
		{
			inputShader.useProgram();
			inputShader.setFloat("time", glfwGetTime());
			glDispatchCompute(ceil(SCREEN_DIMENSION.x/8),ceil(SCREEN_DIMENSION.y/4),1);
			glMemoryBarrier(GL_ALL_BARRIER_BITS);
			//init = false;
		}

		// Clear images (to link with UI)
		if (simulationProperties.reset)
		{
			glClearTexImage(texture1.getTextureID(), 0, GL_RGBA, GL_FLOAT, 0);
			glClearTexImage(texture0.getTextureID(), 0, GL_RGBA, GL_FLOAT, 0);
			simulationProperties.reset = false;
			init = true;
		}

		// Simulation presets 1
		//simulationProperties.speed = 0.5;
		//simulationProperties.diffusionRateA = 1.1211;
		//simulationProperties.diffusionRateB = .566;
		//simulationProperties.feedRate = .044;
		//simulationProperties.killRate = .061;
		// Simulation presets 2
		//simulationProperties.diffusionRateA = 0.933;
		//simulationProperties.diffusionRateB = .21;
		//simulationProperties.feedRate = .023;
		//simulationProperties.killRate = .049;

		computeShader.useProgram();
		computeShader.setFloat("t", currentFrame);

		glm::vec4 properties = {
			simulationProperties.diffusionRateA,
			simulationProperties.diffusionRateB,
			simulationProperties.feedRate,
			simulationProperties.killRate
		};

		computeShader.setVec4("_properties", properties);
		computeShader.setFloat("_DiffusionRateA", simulationProperties.diffusionRateA);
		computeShader.setFloat("_DiffusionRateB", simulationProperties.diffusionRateB);
		computeShader.setFloat("_FeedRate", simulationProperties.feedRate);
		computeShader.setFloat("_KillRate", simulationProperties.killRate);
		glDispatchCompute(ceil(SCREEN_DIMENSION.x/8),ceil(SCREEN_DIMENSION.y/4),1);
		glMemoryBarrier(GL_ALL_BARRIER_BITS);

		// Compute next diffusion rate step
		if (currentTexture == 1)
			texture1.useTexture(0);
		else
			texture0.useTexture(0);
		outTexture.useTexture(1);

		// Apply color computation to the image
		displayShader.useProgram();
		displayShader.setVec3("colorA", simulationProperties.colorA);
		displayShader.setVec3("colorB", simulationProperties.colorB);
		glDispatchCompute(ceil(SCREEN_DIMENSION.x/8),ceil(SCREEN_DIMENSION.y/4),1);
		glMemoryBarrier(GL_ALL_BARRIER_BITS);


		shader.useProgram();
		outTexture.useTexture(0);
		ui.update();

		shader.setInt("screen", GL_TEXTURE0);
		plane.render();

		currentTexture = !currentTexture;

		ui.render();

		glfwSwapBuffers(_window);
		glfwPollEvents();
	}

	glfwTerminate();
}
