#pragma once

#include "../../inc/include.hpp"
#include "../Graphic/ReactionDiffusion/ReactionDiffusionSimulator.hpp"
#include "../Preset.hpp"

class Preset;
class ReactionDiffusionSimulator;

class PresetUI {
private:
	ReactionDiffusionSimulator& _RDSimulator;
	Preset& _presetManager;

public:
	PresetUI(ReactionDiffusionSimulator& RDSimulator, Preset& presetManager);

	void print();
};
