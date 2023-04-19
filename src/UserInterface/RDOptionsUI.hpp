#pragma once

#include "../../inc/include.hpp"
#include "../Graphic/ReactionDiffusion/ReactionDiffusionSimulator.hpp"

class ReactionDiffusionSimulator;

class RDOptionsUI {
private:
	ReactionDiffusionSimulator& _RDSimulator;

	const std::map<std::string, glm::vec2>& _slidersRanges;

	void printNumberTypeFields(const int& i);
	void printNoiseFields(const int& i);
	std::string getFieldNameFromIndex(const int& index) const;

	glm::vec2 getSliderRangeFromOptionIndex(const int& optionIndex) const;

public:
	RDOptionsUI(ReactionDiffusionSimulator& RDSimulator, const std::map<std::string, glm::vec2>& slidersRanges);

	void print(const int& i);
};
