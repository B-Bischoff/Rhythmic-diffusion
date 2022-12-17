#include "UserInterface.hpp"

UserInterface::UserInterface(GLFWwindow& window, const int& winWidth, const int& winHeight, const int& uiWitdth, SimulationProperties& simulationProperties)
	: _window(window), WIN_WIDTH(winWidth), WIN_HEIGHT(winHeight), UI_WIDTH(uiWitdth), _simulationProperties(simulationProperties)
{
#if defined(IMGUI_IMPL_OPENGL_ES2)
	const char* glsl_version = "#version 100";
#elif defined(__APPLE__)
	const char* glsl_version = "#version 150";
#else
	const char* glsl_version = "#version 130";
#endif
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	ImGui::StyleColorsDark();
	ImGui_ImplGlfw_InitForOpenGL(&_window, true);
	ImGui_ImplOpenGL3_Init(glsl_version);
}

void UserInterface::createNewFrame()
{
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
}

void UserInterface::update()
{
	const int PANNEL_WIDTH = UI_WIDTH;
	const int PANNEL_HEIGHT = WIN_HEIGHT;

	ImGuiStyle& style = ImGui::GetStyle();
	style.WindowBorderSize = 0;

	ImGui::SetNextWindowPos(ImVec2(WIN_WIDTH - PANNEL_WIDTH, 0.0f));
	ImGui::SetNextWindowSize(ImVec2(PANNEL_WIDTH, PANNEL_HEIGHT));

	ImGuiWindowFlags windowFlag = 0;
	windowFlag |= ImGuiWindowFlags_NoMove;
	windowFlag |= ImGuiWindowFlags_NoResize;
	//windowFlag |= ImGuiWindowFlags_NoCollapse;
	//windowFlag |= ImGuiWindowFlags_NoTitleBar;

	ImGui::Begin("Menu", NULL, windowFlag);

	ImGui::SliderFloat("Speed", &_simulationProperties.speed, 0.01f, 1.05);
	ImGui::SliderFloat("Diffusion rate A", &_simulationProperties.diffusionRateA, 0.01f, 2.0);
	ImGui::SliderFloat("Diffusion rate B", &_simulationProperties.diffusionRateB, 0.01f, 2.0);
	ImGui::SliderFloat("Feed rate", &_simulationProperties.feedRate, 0.01f, 0.15);
	ImGui::SliderFloat("kill rate", &_simulationProperties.killRate, 0.01f, 0.15);
	static float color1[3] = {0.0f, 0.0f, 0.0f};
	static float color2[3] = {1.0f, 1.0f, 1.0f};
	ImGui::ColorEdit3("Color A", color1);
	_simulationProperties.colorA = glm::vec3(color1[0], color1[1], color1[2]);
	ImGui::ColorEdit3("Color B", color2);
	_simulationProperties.colorB = glm::vec3(color2[0], color2[1], color2[2]);
	if (ImGui::Button("Reset"))
		_simulationProperties.reset = true;


	ImGui::End();
}


void UserInterface::render()
{
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void UserInterface::shutdown()
{
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
}
