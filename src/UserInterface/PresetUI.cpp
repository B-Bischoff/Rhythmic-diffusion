#include "./PresetUI.hpp"

PresetUI::PresetUI(ReactionDiffusionSimulator& RDSimulator, Preset& presetManager)
	: _RDSimulator(RDSimulator), _presetManager(presetManager)
{
}

void PresetUI::print()
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
