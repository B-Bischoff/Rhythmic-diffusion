#include "./UserInterface.hpp"

UserInterface::UserInterface(GLFWwindow& window, const int& winWidth, const int& winHeight, const int& uiWitdth, ReactionDiffusionSimulator& RDSimulator, AudioPlayer& audioPlayer, AudioAnalyzer& audioAnalyzer, Adapter& adapter, Preset& presetManager)
	: _window(window), WIN_WIDTH(winWidth), WIN_HEIGHT(winHeight), UI_WIDTH(uiWitdth), _RDSimulator(RDSimulator), _audioPlayer(audioPlayer), _audioAnalyzer(audioAnalyzer), _adapter(adapter), _presetManager(presetManager),
	_audioPlayerUI(_fileDialog, _audioPlayer, _audioAnalyzer),
	_hooksUI(_RDSimulator, _adapter),
	_initialConditionsUI(_RDSimulator),
	_presetUI(_RDSimulator, _presetManager),
	_RDOptionsUI(_RDSimulator),
	_gradientUI(_RDSimulator, _gradient)
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

UserInterface::~UserInterface()
{
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

	ImGui::Begin("Simulation properties", NULL, windowFlag);

	if (ImGui::Button("clear simulation"))
		_RDSimulator.resetSimulation();
	static float speed = 1.0f;
	if (ImGui::SliderFloat("simulation speed", &speed, 0.00f, 10.0))
		_RDSimulator.setSimulationSpeed(speed);
	ImGui::Separator();

	printAudioPlayer();
	ImGui::Separator();

	ImGuiTabBarFlags tab_bar_flags = ImGuiTabBarFlags_None;
	if (ImGui::BeginTabBar("MyTabBar", tab_bar_flags))
	{
		if (ImGui::BeginTabItem("Reaction diffusion"))
		{
			for (int i = 0; i < 4; i++)
				_RDOptionsUI.print(i);
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
		if (ImGui::BeginTabItem("Gradient"))
		{
			_gradientUI.print();
			ImGui::EndTabItem();
		}

		if (ImGui::BeginTabItem("Preset"))
		{
			printPresets();
			ImGui::EndTabItem();
		}
	}

	ImGui::End();

	// File explorer launching audio on selection
	_fileDialog.Display();
	if (_fileDialog.HasSelected())
	{
		_audioPlayer.stopPlaying();
		_audioPlayer.playWavFile(_fileDialog.GetSelected());
		_fileDialog.ClearSelected();
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

void UserInterface::printAudioPlayer()
{
	_audioPlayerUI.print();
}

void UserInterface::printAdapterHook()
{
	_hooksUI.print();
}

void UserInterface::printInitialConditions()
{
	_initialConditionsUI.print();
}

void UserInterface::printPresets()
{
	_presetUI.print();
}

ImGradient& UserInterface::getGradient()
{
	return _gradient;
}
