#pragma once

#include "../inc/include.hpp"
#include "./Graphic/ReactionDiffusionSimulator.hpp"
#include "./Audio/AudioAnalyzer.hpp"

class Adapter {
private:
	ReactionDiffusionSimulator& _RDSimulator;
	AudioAnalyzer& _audioAnalyzer;

	float _bassRatio;
	std::vector<float> _bassRatioHistory;

	void normalizeGroupsOutputs();
	void modifyReactionDiffusion();

public:
	Adapter(ReactionDiffusionSimulator& RDSimulator, AudioAnalyzer& audioAnalyzer);

	void update();
};
