# pragma once

#include "./Graphic/ReactionDiffusion/ReactionDiffusionSimulator.hpp"
#include "./Adapter.hpp"

struct PresetSettings {
	std::vector<AdapterHook> hooks; // Stored in adapter
	std::vector<Parameter> parameters; // Stored in RDS
	std::vector<InitialConditionsShape> shapes; // Stored in RDS
};

class Preset {
private:
	ReactionDiffusionSimulator& _RDSsimulator;
	Adapter& _adapter;

	std::map<std::string, PresetSettings> _presets;

	bool presetExists(const std::string& presetName) const;
	std::string createGenericName() const;

public:
	Preset(ReactionDiffusionSimulator& RDSsimulator, Adapter& _adapter);

	void addPreset(std::string presetName);
	void removePreset(const std::string& presetName);
	void applyPreset(const std::string& presetName);
	std::vector<std::string> getPresetNames() const;
};
