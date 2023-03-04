#include "UserInterface.hpp"

UserInterface::UserInterface(GLFWwindow& window, const int& winWidth, const int& winHeight, const int& uiWitdth, ReactionDiffusionSimulator& RDSimulator, AudioPlayer& audioPlayer, AudioAnalyzer& audioAnalyzer, Adapter& adapter)
	: _window(window), WIN_WIDTH(winWidth), WIN_HEIGHT(winHeight), UI_WIDTH(uiWitdth), _RDSimulator(RDSimulator), _audioPlayer(audioPlayer), _audioAnalyzer(audioAnalyzer), _adapter(adapter)
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

	// SPEED
	static float speed = 1.0f;
	if (ImGui::SliderFloat("Speed", &speed, 0.00f, 2.0))
		_RDSimulator.setSimulationSpeed(speed);

	// COLORS
	static float color1[3] = {0.0f, 0.0f, 0.0f};
	static float color2[3] = {1.0f, 1.0f, 1.0f};
	if (ImGui::ColorEdit3("Color A", color1))
		_RDSimulator.setSimulationColorA(glm::vec3(color1[0], color1[1], color1[2]));
	if (ImGui::ColorEdit3("Color B", color2))
		_RDSimulator.setSimulationColorB(glm::vec3(color2[0], color2[1], color2[2]));

	//ImGui::Separator();

	for (int i = 0; i < 4; i++)
		printOptionsFields(i);

	if (ImGui::Button("Reset"))
		_RDSimulator.resetSimulation();

	// Plot as lines and plot as histogram
	//IMGUI_DEMO_MARKER("Widgets/Plotting/PlotLines, PlotHistogram");
	//static float arr[] = { 0.6f, 0.1f, 1.0f, 0.5f, 0.92f, 0.1f, 0.2f };
	//ImGui::PlotLines("Frame Times", arr, IM_ARRAYSIZE(arr));

	printAudioPlayer();
	printAdapterHook();

	ImGui::End();
}

void UserInterface::printOptionsFields(const int& i)
{
	// Input Type ComboBox
	int inputParameterType = _RDSimulator.getParameterType(i);
	if (ImGui::Combo(std::string(getFieldNameFromIndex(i) + " type").c_str(), &inputParameterType, "number input\0Perlin noise\0Voronoi\0"))
		_RDSimulator.setParameterType(i, static_cast<InputParameterType>(inputParameterType));

	// Parameter preview
	ImGui::SameLine();
	bool showParam = _RDSimulator.getParameterPreview(i);
	if (ImGui::Checkbox(std::string("Show " + std::to_string(i)).c_str(), &showParam))
		_RDSimulator.setParameterPreview(i, showParam);

	// Print input fields according to input type
	if (_RDSimulator.getParameterType(i) == InputParameterType::Number)
	{
		float min = 0.0f;
		float max = i <= 1 ? 1.0f : 0.15f;
		printNumberTypeFields(i, min, max);
	}
	else if (_RDSimulator.getParameterType(i) == InputParameterType::PerlinNoise)
		printPerlinNoiseFields(i);
}

void UserInterface::printNumberTypeFields(const int& i, const float& min, const float& max)
{
	float value = min;
	const std::vector<float>& inputValue = _RDSimulator.getParameterValue(i);
	if (inputValue.size())
		value = inputValue[0];
	std::string fieldName = std::string("value " + std::to_string(i));
	if (ImGui::SliderFloat(fieldName.c_str(), &value, min, max))
		_RDSimulator.setParameterValue(i, std::vector<float>(1, value));
}

void UserInterface::printPerlinNoiseFields(const int& i)
{
	const std::vector<float>& v = _RDSimulator.getParameterValue(i);
	bool valueChanged = false;

	// Scale
	std::string fieldName = std::string("scale " + std::to_string(i));
	float scale = 0.001;
	if (v.size())
		scale = v[0];
	if (ImGui::SliderFloat(fieldName.c_str(), &scale, 0.001, 0.5))
		valueChanged = true;

	// Offset
	fieldName = std::string("offset " + std::to_string(i));
	float offset[2];
	if (v.size() > 2)
	{
		offset[0] = v[1];
		offset[1] = v[2];
	}
	if (ImGui::SliderFloat2(fieldName.c_str(), offset, -5000, 5000))
		valueChanged = true;

	// Strength factor
	fieldName = std::string("Strength factor " + std::to_string(i));
	float strengthFactor = 1.0f;
	if (v.size() > 3)
		strengthFactor = v[3];
	if (ImGui::SliderFloat(fieldName.c_str(), &strengthFactor, 0.01, 1.5))
		valueChanged = true;

	if (valueChanged)
	{
		std::vector<float> values = { scale, offset[0], offset[1], strengthFactor };
		_RDSimulator.setParameterValue(i, values);
	}
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
	// PAUSE / RESUME
	ImGui::Text("audio file: %s", _audioPlayer.getFileName().c_str());
	if (ImGui::Button("Pause/Resume"))
		_audioPlayer.togglePause();

	// VOLUME
	float volume = _audioPlayer.getVolume();
	ImGui::SliderFloat("volume", &volume, 0, 100);
	if (volume != _audioPlayer.getVolume())
		_audioPlayer.setVolume(volume);

	// TIMESTAMP
	float audioDuration = _audioPlayer.getWavFileDuration() / 44100.0; // In seconds
	float currentDuration = _audioPlayer.getCurrentTimestamp() / 44100.0; // In seconds
	float modifiedDuration = currentDuration;
	ImGui::SliderFloat("timestamp", &modifiedDuration, 0, audioDuration);
	if (modifiedDuration != currentDuration)
		_audioPlayer.setTimestamp((modifiedDuration - currentDuration) * 16384);

	// GRAPH
	const int ARRAY_SIZE = _audioAnalyzer.getOutputArraySize();
	float ARRAY_2[ARRAY_SIZE];
	//std::lock_guard<std::mutex> guard(_audioAnalyzer._outputArrayMutex);
	const std::vector<float>& outputFreq = _audioAnalyzer.getFrequencies();

	if ((int)outputFreq.size() >= ARRAY_SIZE)
	{
		for (int i = 0; i < ARRAY_SIZE; i++)
			ARRAY_2[i] = outputFreq[i];
		ImGui::PlotHistogram("Histogram", ARRAY_2, ARRAY_SIZE, 0, NULL, 0.0f, 50.0f, ImVec2(500, 80.0f));
	}
}

void UserInterface::printAdapterHook()
{
	// Clear hooks
	if (ImGui::Button("clear hooks"))
		_adapter.clearHooks();

	static int audioTrigger = 0;
	ImGui::Combo("audio trigger", &audioTrigger, "bass\0snare\0lead\0");

	static int propertie = 0;
	ImGui::SliderInt("propertie", &propertie, 0, 3);

	static int propertieIndex = 0;
	ImGui::SliderInt("propertie index", &propertieIndex, 0, 4);

	static int actionMode = 0;
	ImGui::Combo("action mode", &actionMode, "add\0subtract\0multiply\0divide\0");

	static float initialValue = 0.0;
	ImGui::SliderFloat("intial value", &initialValue, 0.0, 1.0);

	static float value = 0.0;
	ImGui::SliderFloat("value", &value, 0.0, 10.0);

	if (ImGui::Button("add hook"))
		_adapter.createHook((AudioTrigger)audioTrigger, propertie, propertieIndex, (ActionMode)actionMode, initialValue, value);

	// Print existing hooks
	std::vector<AdapterHook>& hooks = _adapter.getHooks();
	for (int i = 0; i < (int)hooks.size(); i++)
	{
		ImGui::Text("hook: %d", i);
		std::string str;

		int hookAudioTrigger = hooks[i].audioTrigger;
		str = "hook audio trigger " + std::to_string(i);
		if (ImGui::Combo(str.c_str(), &hookAudioTrigger, "bass\0snare\0lead\0"))
			hooks[i].audioTrigger = (AudioTrigger)hookAudioTrigger;

		int hookPropertie = hooks[i].reactionPropertie;
		str = "hook propertie " + std::to_string(i);
		if (ImGui::SliderInt(str.c_str(), &hookPropertie, 0, 3))
			hooks[i].reactionPropertie = hookPropertie;

		int hookPropertieIndex = hooks[i].propertieIndex;
		str = "hook propertie index " + std::to_string(i);
		if (ImGui::SliderInt(str.c_str(), &hookPropertieIndex, 0, 4))
			hooks[i].propertieIndex = hookPropertieIndex;

		int hookActionMode = hooks[i].actionMode;
		str = "hook action mode " + std::to_string(i);
		if (ImGui::Combo(str.c_str(), &hookActionMode, "add\0subtract\0multiply\0divide\0"))
			hooks[i].actionMode = (ActionMode)hookActionMode;

		float hookInitialValue = hooks[i].simulationInitialValue;
		str = "hook initial value " + std::to_string(i);
		if (ImGui::SliderFloat(str.c_str(), &hookInitialValue, 0.0, 3.0))
			hooks[i].simulationInitialValue = hookInitialValue;

		float hookValue = hooks[i].value;
		str = "hook value " + std::to_string(i);
		if (ImGui::SliderFloat(str.c_str(), &hookValue, 0.0, 30.0))
			hooks[i].value = hookValue;
	}
}
