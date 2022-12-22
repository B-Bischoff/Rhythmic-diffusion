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

	ImGui::Begin("Simulation properties", NULL, windowFlag);

	ImGui::SliderFloat("Speed", &_simulationProperties.speed, 0.01f, 1.05);

	static float color1[3] = {0.0f, 0.0f, 0.0f};
	static float color2[3] = {1.0f, 1.0f, 1.0f};
	ImGui::ColorEdit3("Color A", color1);
	_simulationProperties.colorA = glm::vec3(color1[0], color1[1], color1[2]);
	ImGui::ColorEdit3("Color B", color2);
	_simulationProperties.colorB = glm::vec3(color2[0], color2[1], color2[2]);
	//ImGui::Separator();

	for (int i = 0; i < 4; i++)
		printOptionsFields(i);

	if (ImGui::Button("Reset"))
		_simulationProperties.reset = true;


	ImGui::End();
}

void UserInterface::printOptionsFields(const int& i)
{
	// Combo for input type
	int inputParameterType = _inputParameters[i]->getType();
	ImGui::Combo(std::string(getFieldNameFromIndex(i) + " type").c_str(), &inputParameterType, "number input\0Perlin noise\0Voronoi\0");
	if (inputParameterType != (int)_inputParameters[i]->getType())
		_inputParameters[i]->changeType(inputParameterType);
	ImGui::SameLine();
	bool showParam = _simulationProperties.paramTextureVisu[i];
	ImGui::Checkbox(std::string("Show " + std::to_string(i)).c_str(), &showParam);
	_simulationProperties.paramTextureVisu[i] = showParam;

	// Print input fields accord to input type
	if (_inputParameters[i]->getType() == InputParameterType::Number)
	{
		float min = 0.0f;
		float max = i <= 1 ? 1.0f : 0.15f;
		printNumberTypeFields(i, min, max);
	}
}

void UserInterface::printNumberTypeFields(const int& i, const float& min, const float& max)
{
	std::vector<float>& v = _inputParameters[i]->getVectorParameters();
	float value;
	if (v.size())
		value = v[0];
	std::string fieldName = std::string("value " + std::to_string(i));
	ImGui::SliderFloat(fieldName.c_str(), &value, min, max);
	v.clear();
	v.push_back(value);
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

void UserInterface::setInputParameters(InputParameter* p0, InputParameter* p1, InputParameter* p2, InputParameter* p3)
{
	_inputParameters[0] = p0;
	_inputParameters[1] = p1;
	_inputParameters[2] = p2;
	_inputParameters[3] = p3;
}

std::string UserInterface::getFieldNameFromIndex(const int& index) const
{
	switch (index)
	{
		case 0: return "Diffusion rate A";
		case 1: return "Diffusion rate B";
		case 2: return "Feed rate";
		case 3: return "Kill rate";
		default: return "";
	}
}
