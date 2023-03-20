#include "Preset.hpp"

Preset::Preset(ReactionDiffusionSimulator& RDSsimulator)
	: _RDSsimulator(RDSsimulator)
{
}

void Preset::addPreset(PresetSettings &preset, const std::string &presetName)
{
	if (presetExists(presetName))
	{
		std::cerr << "[Preset] a preset named \"" << presetName << "\" already exists. Canceling add preset method." << std::endl;
		return;
	}
	_presets[presetName] = preset;
}

void Preset::removePreset(const std::string& presetName)
{
	if (presetExists(presetName))
	{
		std::cerr << "[Preset] a preset named \"" << presetName << "\" already exists. Canceling remove preset method." << std::endl;
		return;
	}
	_presets.erase(presetName);
}

bool Preset::presetExists(const std::string& presetName)
{
	try {
		PresetSettings& existingPreset = _presets.at(presetName);
	}
	catch (const std::out_of_range& e) {
		return true;
	}
	return false;
}

void Preset::applyPreset(const std::string& presetName)
{
}
