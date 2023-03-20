# pragma once

#include "./Graphic/ReactionDiffusion/ReactionDiffusionSimulator.hpp"
#include "./Adapter.hpp"

struct PresetSettings {
	std::vector<AdapterHook> hooks;
	std::vector<Parameter> parameters;
	std::vector<InitialConditionsShape> shapes;
};

class Preset {
private:
	ReactionDiffusionSimulator& _RDSsimulator;

	std::map<std::string, PresetSettings> _presets;

	bool presetExists(const std::string& presetName);

public:
	Preset(ReactionDiffusionSimulator& RDSsimulator);

	void addPreset(PresetSettings& preset, const std::string& presetName);
	void removePreset(const std::string& presetName);
	void applyPreset(const std::string& presetName);
};
