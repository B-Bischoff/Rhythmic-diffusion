# pragma once

#include "./Graphic/ReactionDiffusion/ReactionDiffusionSimulator.hpp"
#include "./Adapter.hpp"

struct PresetSettings {
	std::vector<AdapterHook> hooks; // Stored in adapter
	std::vector<Parameter> parameters; // Stored in RDS
	std::vector<InitialConditionsShape> shapes; // Stored in RDS

	template<class Archive>
	void serialize(Archive& archive)
	{
		archive(CEREAL_NVP(hooks), CEREAL_NVP(parameters), CEREAL_NVP(shapes));
	}
};

class Preset {
private:
	ReactionDiffusionSimulator& _RDSsimulator;
	Adapter& _adapter;

	const std::string PRESET_EXTENSION = ".preset";
	const std::string PRESET_DIRECTORY = "presets/";
	std::map<std::string, PresetSettings> _presets;

	void loadExistingPresets();

	bool presetExists(const std::string& presetName) const;
	std::string createGenericName() const;

public:
	Preset(ReactionDiffusionSimulator& RDSsimulator, Adapter& _adapter);

	void addPreset(std::string presetName);
	void removePreset(const std::string& presetName);
	void applyPreset(const std::string& presetName);
	std::vector<std::string> getPresetNames() const;

	template<class Archive>
	void serialize(Archive& archive);
};
