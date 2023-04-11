#pragma once

#include "./Graphic/ReactionDiffusion/ReactionDiffusionSimulator.hpp"
#include "./Adapter.hpp"

namespace glm {
	template<class Archive> void serialize(Archive& archive, glm::vec4& v) { archive(v.x, v.y, v.z, v.w); }
};

struct PresetSettings {
	std::vector<AdapterHook> hooks; // Stored in adapter
	std::vector<Parameter> parameters; // Stored in RDS
	std::vector<InitialConditionsShape> shapes; // Stored in RDS
	std::vector<glm::vec4> gradient; // Stored in PostProcessing

	template<class Archive>
	void serialize(Archive& archive)
	{
		archive(CEREAL_NVP(hooks), CEREAL_NVP(parameters), CEREAL_NVP(shapes), CEREAL_NVP(gradient));
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
