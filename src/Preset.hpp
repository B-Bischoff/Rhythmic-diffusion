#pragma once

#include "./Graphic/ReactionDiffusion/ReactionDiffusionSimulator.hpp"
#include "./Adapter.hpp"

namespace glm {
	template<class Archive> void serialize(Archive& archive, glm::vec4& v) { archive(v.x, v.y, v.z, v.w); }
	template<class Archive> void serialize(Archive& archive, glm::vec2& v) { archive(v.x, v.y); }
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

	// Automatic preset switch
	bool _automaticPresetSwitch;
	std::string _currentPreset;
	double _presetSwitchDelay;
	std::chrono::system_clock::time_point _previousTime;

	const std::string PRESET_EXTENSION = ".preset";
#ifdef WIN32
	const std::string PRESET_DIRECTORY = "../../../presets/";
#else
	const std::string PRESET_DIRECTORY = "presets/";
#endif
	std::map<std::string, PresetSettings> _presets;

	void loadExistingPresets();

	bool presetExists(const std::string& presetName) const;
	std::string createGenericName() const;

public:
	Preset(ReactionDiffusionSimulator& RDSsimulator, Adapter& _adapter);

	void addPreset(std::string presetName);
	void removePreset(const std::string& presetName);
	void applyPreset(const std::string& presetName);
	void overwritePreset(const std::string& presetName);
	std::vector<std::string> getPresetNames() const;

	void updateAutomaticPresetSwitch();
	void setAutomaticSwitchDelay(const float& switchDelay);
	void startAutomaticPresetSwitch();
	void stopAutomaticPresetSwitch();
	bool getAutomaticPresetSwitchState() const;
	std::string getCurrentPreset() const;

	template<class Archive>
	void serialize(Archive& archive);
};
