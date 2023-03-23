#include "UserInterface.hpp"
#include "vendor/imgui/imgui.h"
#include <cstring>

UserInterface::UserInterface(GLFWwindow& window, const int& winWidth, const int& winHeight, const int& uiWitdth, ReactionDiffusionSimulator& RDSimulator, AudioPlayer& audioPlayer, AudioAnalyzer& audioAnalyzer, Adapter& adapter, Preset& presetManager)
	: _window(window), WIN_WIDTH(winWidth), WIN_HEIGHT(winHeight), UI_WIDTH(uiWitdth), _RDSimulator(RDSimulator), _audioPlayer(audioPlayer), _audioAnalyzer(audioAnalyzer), _adapter(adapter), _presetManager(presetManager)
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

	_fileDialog.SetTitle("Select audio file");
	_fileDialog.SetTypeFilters({".wav"});
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


	ImGuiTabBarFlags tab_bar_flags = ImGuiTabBarFlags_None;
	if (ImGui::BeginTabBar("MyTabBar", tab_bar_flags))
	{
		if (ImGui::BeginTabItem("Global"))
		{
			// SPEED
			static float speed = 1.0f;
			if (ImGui::SliderFloat("Speed", &speed, 0.00f, 10.0))
				_RDSimulator.setSimulationSpeed(speed);

			// COLORS
			static float color1[3] = {0.0f, 0.0f, 0.0f};
			static float color2[3] = {1.0f, 1.0f, 1.0f};
			if (ImGui::ColorEdit3("Color A", color1))
				_RDSimulator.setSimulationColorA(glm::vec3(color1[0], color1[1], color1[2]));
			if (ImGui::ColorEdit3("Color B", color2))
				_RDSimulator.setSimulationColorB(glm::vec3(color2[0], color2[1], color2[2]));
			if (ImGui::Button("Reset"))
				_RDSimulator.resetSimulation();
			ImGui::EndTabItem();
		}

		if (ImGui::BeginTabItem("RD"))
		{
			for (int i = 0; i < 4; i++)
				printOptionsFields(i);
			ImGui::EndTabItem();
		}
		if (ImGui::BeginTabItem("Audio"))
		{
			printAudioPlayer();
			ImGui::EndTabItem();
		}

		if (ImGui::BeginTabItem("Hooks"))
		{
			printAdapterHook();
			ImGui::EndTabItem();
		}

		if (ImGui::BeginTabItem("Shapes"))
		{
			printInitialConditions();
			ImGui::EndTabItem();
		}

		if (ImGui::BeginTabItem("Preset"))
		{
			printPresets();
			ImGui::EndTabItem();
		}
	}

	ImGui::End();

	_fileDialog.Display();
	if (_fileDialog.HasSelected())
	{
		_audioPlayer.stopPlaying();
		_audioPlayer.playWavFile(_fileDialog.GetSelected());
		_fileDialog.ClearSelected();
	}
}

void UserInterface::printOptionsFields(const int& i)
{
	// Input Type ComboBox
	int inputParameterType = _RDSimulator.getParameterType(i);
	if (ImGui::Combo(std::string(getFieldNameFromIndex(i) + " type").c_str(), &inputParameterType, "number input\0Perlin noise\0Voronoi\0"))
		_RDSimulator.setParameterType(i, static_cast<InputParameterType>(inputParameterType));

	// Parameter preview
	//ImGui::SameLine();
	bool showParam = _RDSimulator.getParameterPreview(i);
	if (ImGui::Checkbox(std::string("Show " + std::to_string(i)).c_str(), &showParam))
		_RDSimulator.setParameterPreview(i, showParam);

	// Print input fields according to input type
	const InputParameterType& parameterType = _RDSimulator.getParameterType(i);
	if (parameterType == Number)
	{
		float min = 0.0f;
		float max = i <= 1 ? 1.0f : 0.1f;
		printNumberTypeFields(i, min, max);
	}
	else if (parameterType == PerlinNoise || parameterType == Voronoi)
		printNoiseFields(i);
}

void UserInterface::printNumberTypeFields(const int& i, const float& min, const float& max)
{
	float value = min;
	std::vector<float> inputValue;
	try {
		inputValue = _RDSimulator.getParameterValue(i);
	}
	catch (std::out_of_range& e) {
		//std::cout << "error" << std::endl;
		inputValue.push_back(0);
	}
	if (inputValue.size())
		value = inputValue[0];
	std::string fieldName = std::string("value " + std::to_string(i));
	if (ImGui::SliderFloat(fieldName.c_str(), &value, min, max,"%.5f"))
		_RDSimulator.setParameterValue(i, std::vector<float>(1, value));
}

void UserInterface::printNoiseFields(const int& i)
{
	const std::vector<float>& v = _RDSimulator.getParameterValue(i);
	bool valueChanged = false;

	std::string fieldName;

	// Strength factor
	fieldName = std::string("Strength factor " + std::to_string(i));
	float strengthFactor = 1.0f;
	strengthFactor = v[0];
	if (ImGui::SliderFloat(fieldName.c_str(), &strengthFactor, 0.01, 3.5))
		valueChanged = true;

	// Scale
	fieldName = std::string("scale " + std::to_string(i));
	float scale = 0.001;
	scale = v[1];
	if (ImGui::SliderFloat(fieldName.c_str(), &scale, 0.001, 1.0))
		valueChanged = true;

	// Offset
	fieldName = std::string("offset " + std::to_string(i));
	float offset[2];
	offset[0] = v[2];
	offset[1] = v[3];
	if (ImGui::SliderFloat2(fieldName.c_str(), offset, -5000, 5000))
		valueChanged = true;

	// Change scale depending on time
	fieldName = std::string("Scale moving " + std::to_string(i));
	bool movingScale = (bool)v[4];
	if (ImGui::Checkbox(fieldName.c_str(), &movingScale))
		valueChanged = true;

	// Time multiplier
	fieldName = std::string("Time multiplier" + std::to_string(i));
	float timeMultiplier = v[5];
	if (ImGui::SliderFloat(fieldName.c_str(), &timeMultiplier, 0.0, 1.0))
		valueChanged = true;

	if (valueChanged)
	{
		std::vector<float> values = { strengthFactor, scale, offset[0], offset[1], (movingScale ? 1.0f : 0.0f), timeMultiplier };
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
	const std::string downloadPath = "../../Downloads/wav/";
	std::string files[] {"Le-Wanski-M.U.S.H..wav",
	"花伦HuaLun-坏孩子的天空.wav",
	"Dax-J-Reign-Of-Terror-_EDLX051_.wav",
	"Sköne-Fin-de-ce-qui-est-relatif-à-la-Nature_-au-Temps_-à-la-Conscience-et-aux-Perspectives-_2h44_.wav",
	"I-Hate-Models-Daydream-_ARTS020_.wav",
	"I-Hate-Models-Shades-of-Night-_ARTS020_.wav",
	"I-Hate-Models-Izanami-_ARTSBOX001_.wav",
	"I-HATE-MODELS-Sorrows-of-the-Moon-_ARTSCORE002_.wav",
	"Mandragora-Codeine-_Original-Mix_.wav",
	"BICEP-_-HAMMER-DAHLIA.wav",
	"UICIDEBOY_-ANTARCTICA.wav",
	"Sunshine.wav",
	"UICIDEBOY_-CHARIOT-OF-FIRE.wav",
	"UICIDEBOY_-I-NO-LONGER-FEAR-THE-RAZOR-GUARDING-MY-HEEL-I-_-II-_-III-_-IV.wav",
	"UICIDEBOY_-O-Lord_-I-Have-My-Doubts.wav",
	"Digital-Baptism.wav",
	"Xaoc.wav",
	"Panoramic-Feelings.wav",
	"MilkyWay-_Explore_.wav",
	"Those-Who-Ride-With-Giants-Those-Who-Ride-With-Giants-_Deluxe_-_Full-Album_.wav",
	"ALT236-ALL-SEEING-EYE-Music-Video-by-DON-TANCREDO.wav",
	"A-S-Y-S-_-Kai-Tracid-Rave-The-Planet-_Original-Mix_.wav",
	"HI-LO-x-Space-92-Mercury-_Original-Mix_.wav",
	"Jeff-Mills-Flying-Machines.wav",
	"Le-Wanski-Clapotis.wav",
	"Pawlowski-Demonic-Dimensions-_POSS001_.wav",
	"Caravel-HÖR-Nov-29-2022.wav",
	"Jacques-Brel-La-valse-à-mille-temps.wav",
	"Cassie-Raptor-HÖR-Jan-25-2023.wav",
	};

	// Play
	static int fileToPlay = 0;
	ImGui::Combo("fileList", &fileToPlay, "mush\0hualun\0daxj\0skone\0IHM-daydream\0IHM-shades\0IHM-izanami\0IHM-moon\0codeine\0dahlia\0antarctica\0sunshine\0chariotOfFire\0iNoLongFear\0OLord\0digitalBaptism\0xaoc\0panoramic\0milyWay\0thosewhoride\0alt236\0asys\0hilo\0jeffmills\0clapotis\0pawloski\0caravel\0jacques\0Cassie-Raptor\0");
	if (ImGui::Button("play file"))
	{
		_audioPlayer.stopPlaying();
		_audioPlayer.playWavFile(downloadPath + files[fileToPlay]);
	}

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

	if (ImGui::Button("open file explorer"))
		_fileDialog.Open();
}

void UserInterface::printAdapterHook()
{
	// Clear hooks
	if (ImGui::Button("clear hooks"))
		_adapter.clearHooks();

	static int audioTrigger = 0;
	ImGui::Combo("audio trigger", &audioTrigger, "bass\0snare\0lead\0");

	static int propertie = 0;
	const int ITEMS_NUMBER = 4 + (int)_RDSimulator.getInitialConditionsShapes().size();
	const char* items[] = { "RdA", "RdB", "Feed", "Kill", "Shape 0", "Shape 1", "Shape 2", "Shape 3",
		"Shape 4", "Shape 5", "Shape 6", "Shape 7", "Shape 8", "Shape 9", "Shape 10", "Shape 11", "Shape 12", "Shape 13", "Shape 14", "Shape 15", "Shape 16" };

	ImGui::Combo("propertie", &propertie, items, ITEMS_NUMBER);

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
		if (ImGui::Combo(str.c_str(), &hookPropertie, items, ITEMS_NUMBER))
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
		if (ImGui::SliderFloat(str.c_str(), &hookValue, 0.0, 3.0))
			hooks[i].value = hookValue;

		str = "erase hook " + std::to_string(i);
		if (ImGui::Button(str.c_str()))
		{
			_adapter.removeHook(i);
			return;
		}
	}
}

void UserInterface::printInitialConditions()
{
	static int shape = 0;
	const char* shapesElements = "circle\0triangle\0hexagon\0square\0";
	ImGui::Combo("shape shape", &shape, shapesElements);

	static float radius = 0;
	ImGui::SliderFloat("shape radius", &radius, 0, 1000);

	static float borderSize = 0;
	ImGui::SliderFloat("shape border", &borderSize, 0, 1000);

	static float angle = 0;
	ImGui::SliderFloat("shape angle", &angle, 0, 360);

	if (ImGui::Button("add shape"))
		_RDSimulator.addInitialConditionsShape(InitialConditionsShape((Shape)shape, radius, borderSize, angle));

	// Print existing shapes
	std::vector<InitialConditionsShape>& shapes = _RDSimulator.getInitialConditionsShapes();

	for (int i = 0; i < (int)shapes.size(); i++)
	{
		ImGui::Text("shape: %d", i);
		std::string str;

		int shapeShape = (int)shapes[i].shape;
		str = "shape " + std::to_string(i);
		if (ImGui::Combo(str.c_str(), &shapeShape, shapesElements))
			shapes[i].shape = (Shape)shapeShape;

		float shapeRadius = shapes[i].radius;
		str = "radius " + std::to_string(i);
		if (ImGui::SliderFloat(str.c_str(), &shapeRadius, 0, 1000))
			shapes[i].radius = shapeRadius;

		float shapeBorder = shapes[i].borderSize;
		str = "border " + std::to_string(i);
		if (ImGui::SliderFloat(str.c_str(), &shapeBorder, 0, 1000))
			shapes[i].borderSize = shapeBorder;

		float shapeAngle = shapes[i].rotationAngle;
		str = "angle " + std::to_string(i);
		if (ImGui::SliderFloat(str.c_str(), &shapeAngle, 0, 360))
			shapes[i].rotationAngle = shapeAngle;

		str = "erase shape " + std::to_string(i);
		if (ImGui::Button(str.c_str()))
		{
			_RDSimulator.removeInitialConditionsShape(i);
			return;
		}
	}
}

void UserInterface::printPresets()
{
	const int presetNameMaxLength = 64;
	static char presetNameBuffer[presetNameMaxLength] = "";
	ImGui::SetNextItemWidth(200);
	ImGui::InputText("preset name (leave blank for generic name)", presetNameBuffer, presetNameMaxLength);

	if (ImGui::Button("Save preset"))
	{
		_presetManager.addPreset(std::string(presetNameBuffer));
		memset(presetNameBuffer, 0, size_t(presetNameMaxLength));
	}

	std::vector<std::string> presetNames = _presetManager.getPresetNames();

	for (int i = 0; i < (int)presetNames.size(); i++)\
	{
		std::string buttonText;
		buttonText = ("Load " + std::to_string(i));
		if (ImGui::Button(buttonText.c_str()))
			_presetManager.applyPreset(presetNames[i]);
		ImGui::SameLine();
		ImGui::Text("%s", presetNames[i].c_str());

		// Do not create erase button when name contains "preset"
		if (presetNames[i].find("preset") != std::string::npos)
			continue;

		ImGui::SameLine();
		buttonText = ("Erase " + std::to_string(i));
		if (ImGui::Button(buttonText.c_str()))
			_presetManager.removePreset(presetNames[i]);
	}
}
