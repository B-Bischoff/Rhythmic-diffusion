#include "./HooksUI.hpp"

HooksUI::HooksUI(ReactionDiffusionSimulator& RDSimulator, Adapter& adapter)
	: _RDSimulator(RDSimulator), _adapter(adapter)
{
}

void HooksUI::print()
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
