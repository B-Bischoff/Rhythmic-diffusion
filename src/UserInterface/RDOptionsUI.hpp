#pragma once

#include "../../inc/include.hpp"
#include "../Graphic/ReactionDiffusion/ReactionDiffusionSimulator.hpp"

class ReactionDiffusionSimulator;

class RDOptionsUI {
private:
	ReactionDiffusionSimulator& _RDSimulator;

	void printNumberTypeFields(const int& i, const float& min, const float& max);
	void printNoiseFields(const int& i);
	std::string getFieldNameFromIndex(const int& index) const;

public:
	RDOptionsUI(ReactionDiffusionSimulator& RDSimulator);

	void print(const int& i);
};
