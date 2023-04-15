#pragma once

#include "./Graphic/ReactionDiffusion/ReactionDiffusionSimulator.hpp"
#include "./Audio/AudioAnalyzer.hpp"

enum ActionMode { add, subtract, multiply, divide };
enum AudioTrigger { bass, snare, lead };

/*
 * AdapterHook allow adapter object to "link" audio trigger (bass, snare, lead)
 * with actions to perform on the diffusion reaction simulation
 *
 * The actions are used to increase or decrease a value of the simulation.
 * The reaction propertie index allows to select a "sub-propertie"
 * Those sub-properties depends on the reaction diffusion input parameter type
*/
struct AdapterHook {
	AudioTrigger audioTrigger;
	int reactionPropertie;
	int propertieIndex;
	ActionMode actionMode;
	double simulationInitialValue;
	double value;

	AdapterHook(const AudioTrigger audioTrigger, const int reactionPropertie, const int propertieIndex, const ActionMode actionMode, const double simulationInitialValue, const double value)
		: audioTrigger(audioTrigger), reactionPropertie(reactionPropertie), propertieIndex(propertieIndex), actionMode(actionMode), simulationInitialValue(simulationInitialValue), value(value) {}
	AdapterHook() {};

	template<class Archive>
	void serialize(Archive& archive)
	{
		archive(CEREAL_NVP(audioTrigger), CEREAL_NVP(reactionPropertie), CEREAL_NVP(propertieIndex), CEREAL_NVP(actionMode), CEREAL_NVP(simulationInitialValue), CEREAL_NVP(value));
	}
};

class Adapter {
private:
	ReactionDiffusionSimulator& _RDSimulator;
	AudioAnalyzer& _audioAnalyzer;

	std::vector<AdapterHook> _hooks;
	float _bassRatio, _snareRatio, _leadRatio;

	void normalizeGroupsOutputs();
	void findBassRatioFromGroup(SoundGroup& group, float& bassMagnitude, float& bassDelta);
	void findSnareRatioFromGroup(SoundGroup& group, float& snareMagnitude);
	void findLeadRatioFromGroup(SoundGroup& group);

	void modifyReactionDiffusion();
	void applyHook(const AdapterHook& hook);
	const float& getRatioReferenceFromAudioTrigger(const AudioTrigger& audioTrigger) const;

public:
	Adapter(ReactionDiffusionSimulator& RDSimulator, AudioAnalyzer& audioAnalyzer);

	void update();
	void createHook(const AudioTrigger audioTrigger, const int reactionPropertie, const int propertieIndex, const ActionMode actionMode, const double simulationInitialValue, const double value);
	void clearHooks();
	void removeHook(const int& index);
	std::vector<AdapterHook>& getHooks();
	glm::vec3 getRatios() const;
};
