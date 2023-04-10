#pragma once

#include "../../inc/include.hpp"

#include "../Adapter.hpp"
#include "../Graphic/ReactionDiffusion/ReactionDiffusionSimulator.hpp"

class Adapter;
class ReactionDiffusionSimulator;

class HooksUI {
private:
	ReactionDiffusionSimulator _RDSimulator;
	Adapter& _adapter;

public:
	HooksUI(ReactionDiffusionSimulator& RDSimulator, Adapter& adapter);

	void print();
};
