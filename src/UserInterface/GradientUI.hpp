#pragma once

#include "../../inc/include.hpp"
#include "../Graphic/ReactionDiffusion/ReactionDiffusionSimulator.hpp"

class ReactionDiffusionSimulator;

class GradientUI {
private:
	ReactionDiffusionSimulator& _RDSimulator;
	ImGradient& _gradient;

	ImGradientMark* _draggingMark;
	ImGradientMark* _selectedMark;

public:
	GradientUI(ReactionDiffusionSimulator& RDSimulator, ImGradient& gradient);

	void print();
};
