#pragma once

#include "../../inc/include.hpp"

#include "../Graphic/ReactionDiffusion/ReactionDiffusionSimulator.hpp"

class ReactionDiffusionSimulator;

class InitialConditionsUI {
private:
	ReactionDiffusionSimulator& _RDSimulator;

	const std::map<std::string, glm::vec2>& _slidersRanges;

public:
	InitialConditionsUI(ReactionDiffusionSimulator& RDSimulator, const std::map<std::string, glm::vec2>& slidersRanges);

	void print();
};
