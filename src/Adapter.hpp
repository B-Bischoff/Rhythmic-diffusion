#pragma once

#include "./Graphic/ReactionDiffusionSimulator.hpp"
#include "./Audio/AudioAnalyzer.hpp"

class Adapter {
private:
	ReactionDiffusionSimulator& _RDSimulator;
	AudioAnalyzer& _audioAnalyzer;

	float _bassRatio;
	float _snareRatio;
	float _leadRatio;

	void normalizeGroupsOutputs();
	void findBassRatioFromGroup(SoundGroup& group, float& bassMagnitude, float& bassDelta);
	void findSnareRatioFromGroup(SoundGroup& group, float& snareMagnitude);
	void findLeadRatioFromGroup(SoundGroup& group, float& leadMagnitude);

	void modifyReactionDiffusion();

public:
	Adapter(ReactionDiffusionSimulator& RDSimulator, AudioAnalyzer& audioAnalyzer);

	void update();
};
