#include "./UserInterface.hpp"

UserInterface::UserInterface(GLFWwindow& window, const int& winWidth, const int& winHeight, const int& uiWitdth, ReactionDiffusionSimulator& RDSimulator, AudioPlayer& audioPlayer, AudioAnalyzer& audioAnalyzer, Adapter& adapter, Preset& presetManager)
	: _window(window), WIN_WIDTH(winWidth), WIN_HEIGHT(winHeight), UI_WIDTH(uiWitdth), _RDSimulator(RDSimulator), _audioPlayer(audioPlayer), _audioAnalyzer(audioAnalyzer), _adapter(adapter), _presetManager(presetManager),
	_audioPlayerUI(_fileBrowser, _audioPlayer, _audioAnalyzer),
	_hooksUI(_RDSimulator, _adapter, _slidersRanges),
	_initialConditionsUI(_RDSimulator, _slidersRanges),
	_presetUI(_RDSimulator, _presetManager),
	_RDOptionsUI(_RDSimulator, _slidersRanges),
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

	_fileBrowser.SetTitle("Select audio file");
	_fileBrowser.SetTypeFilters({".wav"});

	_slidersRanges["RDA"] = { 0.0, 1.0 };
	_slidersRanges["RDB"] = { 0.0, 1.0 };
	_slidersRanges["FeedRate"] = { 0.0, 0.06 };
	_slidersRanges["KillRate"] = { 0.0, 0.06 };
	_slidersRanges["NoiseScale"] = { 0.0, 1.0 };
	_slidersRanges["NoiseOffset"] = { -5000.0, 5000.0 };
	_slidersRanges["ShapesRadius"] = { 0.0, 1000.0 };
	_slidersRanges["ShapesBorder"] = { 0.0, 1000.0 };
	_slidersRanges["ShapesAngle"] = { -360.0, 360.0 };
	_slidersRanges["ShapesOffset"] = { -2000.0, 2000.0 };
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

	ImGui::Text("\nGeneral properties");
	if (ImGui::Button("clear simulation"))
		_RDSimulator.resetSimulation();
	static float speed = 1.0f;
	if (ImGui::SliderFloat("simulation speed", &speed, 0.00f, 10.0))
		_RDSimulator.setSimulationSpeed(speed);
	ImGui::Text("\n");
	ImGui::Separator();

	_audioPlayerUI.print();
	ImGui::Separator();

	ImGui::Text("\n");
	ImGuiTabBarFlags tab_bar_flags = ImGuiTabBarFlags_None;
	if (ImGui::BeginTabBar("MyTabBar", tab_bar_flags))
	{
		if (ImGui::BeginTabItem("Reaction diffusion"))
		{
			ImGui::Text("\n");
			for (int i = 0; i < 4; i++)
			{
				_RDOptionsUI.print(i);
				ImGui::Text("\n");
				ImGui::Separator();
				ImGui::Text("\n");
			}
			ImGui::EndTabItem();
		}

		if (ImGui::BeginTabItem("Hooks"))
		{
			_hooksUI.print();
			ImGui::EndTabItem();
		}

		if (ImGui::BeginTabItem("Shapes"))
		{
			_initialConditionsUI.print();
			ImGui::EndTabItem();

		}
		if (ImGui::BeginTabItem("Gradient"))
		{
			_gradientUI.print();
			ImGui::EndTabItem();
		}

		if (ImGui::BeginTabItem("Preset"))
		{
			_presetUI.print();
			ImGui::EndTabItem();
		}
		ImGui::EndTabBar();
	}

	ImGui::End();

	// File explorer launching audio on selection
	_fileBrowser.Display();
	if (_fileBrowser.HasSelected())
	{
		_audioPlayer.stopPlaying();
		_audioPlayer.playWavFile(_fileBrowser.GetSelected().u8string());
		_fileBrowser.ClearSelected();
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

ImGradient& UserInterface::getGradient()
{
	return _gradient;
}
