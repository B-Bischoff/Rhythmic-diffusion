#include "UserInterface.hpp"
#include "vendor/imgui/imgui.h"
#include "./AudioAnalyzer.hpp"

UserInterface::UserInterface(GLFWwindow& window, const int& winWidth, const int& winHeight, const int& uiWitdth, SimulationProperties& simulationProperties, AudioPlayer& audioPlayer, AudioAnalyzer& audioAnalyzer)
	: _window(window), WIN_WIDTH(winWidth), WIN_HEIGHT(winHeight), UI_WIDTH(uiWitdth), _simulationProperties(simulationProperties), _audioPlayer(audioPlayer), _audioAnalyzer(audioAnalyzer)
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

void UserInterface::update(const float array[600])
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

	// Plot as lines and plot as histogram
	//IMGUI_DEMO_MARKER("Widgets/Plotting/PlotLines, PlotHistogram");
	//static float arr[] = { 0.6f, 0.1f, 1.0f, 0.5f, 0.92f, 0.1f, 0.2f };
	//ImGui::PlotLines("Frame Times", arr, IM_ARRAYSIZE(arr));


	const int ARRAY_SIZE = 600;
	float ARRAY_2[ARRAY_SIZE];
	std::lock_guard<std::mutex> guard(_audioAnalyzer._outputArrayMutex);
	const std::vector<float>& outputFreq = _audioAnalyzer.getFrequencies();
	if (outputFreq.size() >= ARRAY_SIZE)
	{
		for (int i = 0; i < ARRAY_SIZE; i++)
			ARRAY_2[i] = outputFreq[i];
		ImGui::PlotHistogram("Histogram", ARRAY_2, ARRAY_SIZE, 0, NULL, 0.0f, 25.0f, ImVec2(500, 80.0f));
	}
	printAudioPlayer();

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
	else if (_inputParameters[i]->getType() == InputParameterType::PerlinNoise)
		printPerlinNoiseFields(i);
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

void UserInterface::printPerlinNoiseFields(const int& i)
{
	std::vector<float>& v = _inputParameters[i]->getVectorParameters();

	// Scale
	std::string fieldName = std::string("scale " + std::to_string(i));
	float scale = 0.001;
	if (v.size())
		scale = v[0];
	ImGui::SliderFloat(fieldName.c_str(), &scale, 0.001, 0.5);

	// Offset
	fieldName = std::string("offset " + std::to_string(i));
	float offset[2];
	if (v.size() > 2)
	{
		offset[0] = v[1];
		offset[1] = v[2];
	}
	ImGui::SliderFloat2(fieldName.c_str(), offset, -5000, 5000);

	// Strength factor
	fieldName = std::string("Strength factor " + std::to_string(i));
	float strengthFactor = 1.0f;
	if (v.size() > 3)
		strengthFactor = v[3];
	ImGui::SliderFloat(fieldName.c_str(), &strengthFactor, 0.01, 1.5);

	v.clear();
	v.push_back(scale);
	v.push_back(offset[0]);
	v.push_back(offset[1]);
	v.push_back(strengthFactor);
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

void UserInterface::printAudioPlayer()
{
	ImGui::Text("audio file: %s", _audioPlayer.getFileName().c_str());
	if (ImGui::Button("Pause/Resume"))
		_audioPlayer.togglePause();

	float volume = _audioPlayer.getVolume();
	ImGui::SliderFloat("volume", &volume, 0, 100);
	if (volume != _audioPlayer.getVolume())
		_audioPlayer.setVolume(volume);

	float audioDuration = _audioPlayer.getWavFileDuration() / 44100.0; // In seconds
	float currentDuration = _audioPlayer.getCurrentTimestamp() / 44100.0; // In seconds
	float modifiedDuration = currentDuration;
	ImGui::SliderFloat("timestamp", &modifiedDuration, 0, audioDuration);
	if (modifiedDuration != currentDuration)
		_audioPlayer.setTimestamp((modifiedDuration - currentDuration) * 16384);
}
