#include "./PresetUI.hpp"

PresetUI::PresetUI(ReactionDiffusionSimulator& RDSimulator, Preset& presetManager)
	: _RDSimulator(RDSimulator), _presetManager(presetManager)
{
}

void PresetUI::print()
{
	ImGui::Text("\n");
	ImGui::Text("Automatic preset transition");
	const std::string presetSwitchText = _presetManager.getAutomaticPresetSwitchState() ? "Disable auto transition" : "Enable auto transition";

	static float presetSwitchDelay = 5.0;
	ImGui::SetNextItemWidth(200);
	if (ImGui::SliderFloat("time between transitions (in seconds)", &presetSwitchDelay, 2, 120))
		_presetManager.setAutomaticSwitchDelay(presetSwitchDelay);

	if (ImGui::Button(presetSwitchText.c_str()))
	{
		if (_presetManager.getAutomaticPresetSwitchState())
			_presetManager.stopAutomaticPresetSwitch();
		else
		{
			_presetManager.setAutomaticSwitchDelay(presetSwitchDelay);
			_presetManager.startAutomaticPresetSwitch();
		}
	}

	ImGui::Text("\n");
	ImGui::Separator();
	ImGui::Text("\n");

	ImGui::Text("last preset applied: %s", _presetManager.getCurrentPreset().c_str());

	const int presetNameMaxLength = 64;
	static char presetNameBuffer[presetNameMaxLength] = "";
	ImGui::SetNextItemWidth(200);
	ImGui::InputText("preset name (leave blank for generic name)", presetNameBuffer, presetNameMaxLength);

	if (ImGui::Button("Save preset"))
	{
		_presetManager.addPreset(std::string(presetNameBuffer));
		memset(presetNameBuffer, 0, size_t(presetNameMaxLength));
	}
	ImGui::SameLine();
	if (ImGui::Button("Override last preset applied"))
		ImGui::OpenPopup("Overwrite?");

	if (ImGui::BeginPopupModal("Overwrite?", NULL, ImGuiWindowFlags_AlwaysAutoResize))
	{
		ImGui::Text("Those preset properties will be modified.\nThis operation cannot be undone!\n\n");
		ImGui::Separator();

		if (ImGui::Button("OK", ImVec2(120, 0)))
		{
			_presetManager.overwritePreset(_presetManager.getCurrentPreset());
			ImGui::CloseCurrentPopup();
		}
		ImGui::SetItemDefaultFocus();
		ImGui::SameLine();
		if (ImGui::Button("Cancel", ImVec2(120, 0))) { ImGui::CloseCurrentPopup(); }
		ImGui::EndPopup();
	}

	std::vector<std::string> presetNames = _presetManager.getPresetNames();

	static int presetIndex = 0;

	ImGui::Text("\n");
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
		{
			presetIndex = i;
			ImGui::OpenPopup("Delete?");
		}

	}
	if (ImGui::BeginPopupModal("Delete?", NULL, ImGuiWindowFlags_AlwaysAutoResize))
	{
		ImGui::Text("The preset '%s' will be entirely deleted.\nThis operation cannot be undone!\n\n", presetNames[presetIndex].c_str());
		ImGui::Separator();

		if (ImGui::Button("OK", ImVec2(120, 0)))
		{
			_presetManager.removePreset(presetNames[presetIndex]);
			ImGui::CloseCurrentPopup();
		}
		ImGui::SetItemDefaultFocus();
		ImGui::SameLine();
		if (ImGui::Button("Cancel", ImVec2(120, 0))) { ImGui::CloseCurrentPopup(); }
		ImGui::EndPopup();
	}
}
