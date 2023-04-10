#pragma once

#include "../../inc/include.hpp"

#include "../Graphic/ReactionDiffusion/ReactionDiffusionSimulator.hpp"

class ReactionDiffusionSimulator;

class InitialConditionsUI {
private:
	ReactionDiffusionSimulator& _RDSimulator;

public:
	InitialConditionsUI(ReactionDiffusionSimulator& RDSimulator);

	void print();
};
