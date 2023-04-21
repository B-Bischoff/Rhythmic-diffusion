#include "./HooksUI.hpp"

HooksUI::HooksUI(ReactionDiffusionSimulator& RDSimulator, Adapter& adapter, std::map<std::string, glm::vec2>& slidersRanges)
	: _RDSimulator(RDSimulator), _adapter(adapter), _slidersRanges(slidersRanges)
{
}

void HooksUI::print()
{
	ImGui::Text("\n");

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

	if (propertie <= 3) // Rd parameter
	{
		// Number does not have any 'subproperties'
		if (_RDSimulator.getParameterType(propertie) != Number)
		{
			// Noise options
			std::string str = "hook propertie index";
			const char* noiseElements = "strength\0scale\0offset X\0offset Y\0time multiplier\0base value\0";
			ImGui::Combo(str.c_str(), &propertieIndex, noiseElements);
		}
		else
			propertieIndex = 0;
	}
	else // Shape
	{
		std::string str = "shape propertie index";
		const char* shapeElements = "radius\0border\0angle\0offset X\0offset Y\0";
		ImGui::Combo(str.c_str(), &propertieIndex, shapeElements);
	}

	static int actionMode = 0;
	ImGui::Combo("action mode", &actionMode, "add\0subtract\0boolean\0");

	glm::vec2 valueRange = getSliderRangesFromHookPropertie(propertie, propertieIndex);
	static float initialValue = 0.0;
	ImGui::SliderFloat("intial value", &initialValue, valueRange[0], valueRange[1],  "%.5f");

	static float value = 0.0;
	ImGui::SliderFloat("value", &value, valueRange[0], valueRange[1],  "%.5f");

	if (ImGui::Button("add hook"))
		_adapter.createHook((AudioTrigger)audioTrigger, propertie, propertieIndex, (ActionMode)actionMode, initialValue, value);

	ImGui::Text("\n");
	ImGui::Separator();
	ImGui::Text("\n");

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

		displayPropertieIndex(hooks[i], i);

		int hookActionMode = hooks[i].actionMode;
		str = "hook action mode " + std::to_string(i);
		if (ImGui::Combo(str.c_str(), &hookActionMode, "add\0subtract\0boolean\0"))
			hooks[i].actionMode = (ActionMode)hookActionMode;

		glm::vec2 sliderValue = getSliderRangesFromHookPropertie(hooks[i].reactionPropertie, hooks[i].propertieIndex);

		float hookInitialValue = hooks[i].simulationInitialValue;
		str = "hook initial value " + std::to_string(i);
		if (ImGui::SliderFloat(str.c_str(), &hookInitialValue, sliderValue[0], sliderValue[1], "%.5f"))
			hooks[i].simulationInitialValue = hookInitialValue;


		float hookValue = hooks[i].value;
		str = "hook value " + std::to_string(i);
		if (ImGui::SliderFloat(str.c_str(), &hookValue, sliderValue[0], sliderValue[1], "%.5f"))
			hooks[i].value = hookValue;

		str = "erase hook " + std::to_string(i);
		if (ImGui::Button(str.c_str()))
		{
			_adapter.removeHook(i);
			return;
		}
	}
}

void HooksUI::displayPropertieIndex(AdapterHook& hook, const int& i)
{
	if (hook.reactionPropertie <= 3) // Rd parameter
	{
		// Number does not have any 'subproperties'
		if (_RDSimulator.getParameterType(hook.reactionPropertie) == Number)
			return;

		// Noise options
		std::string str = "hook propertie index " + std::to_string(i);
		const char* noiseElements = "strength\0scale\0offset X\0offset Y\0time multiplier\0base value\0";
		ImGui::Combo(str.c_str(), &hook.propertieIndex, noiseElements);
	}
	else // Shape
	{
		std::string str = "shape propertie index " + std::to_string(i);
		const char* shapeElements = "radius\0border\0angle\0offset X\0offset Y\0";
		ImGui::Combo(str.c_str(), &hook.propertieIndex, shapeElements);
	}
}

glm::vec2 HooksUI::getSliderRangesFromHookPropertie(const int& index, const int& propertieIndex) const
{
	if (index <= 3) // RD parameters
	{
		InputParameterType parameterType = _RDSimulator.getParameterType(index);
		if (parameterType == Number || propertieIndex == 0 || propertieIndex == 5) // strength
		{
			switch (index) {
				case 0: return _slidersRanges.at("RDA");
				case 1: return _slidersRanges.at("RDB");
				case 2: return _slidersRanges.at("FeedRate");
				case 3: return _slidersRanges.at("KillRate");
				default:return glm::vec2(0, 1);
			}
		}
		else // Noise property
		{
			switch (propertieIndex) {
				case 0: return _slidersRanges.at("NoiseScale");
				case 1: return _slidersRanges.at("NoiseOffset");
				case 2: return _slidersRanges.at("NoiseOffset");
				case 3: return glm::vec2(0, 1); // Time multiplier
			}
		}
	}
	else // Shapes
	{
		switch (propertieIndex) {
			case 0: return _slidersRanges.at("ShapesRadius");
			case 1: return _slidersRanges.at("ShapesBorder");
			case 2: return _slidersRanges.at("ShapesAngle");
			case 3: return _slidersRanges.at("ShapesOffset");
			case 4: return _slidersRanges.at("ShapesOffset");
		}
	}

	// temp
	return glm::vec2(0);
}
