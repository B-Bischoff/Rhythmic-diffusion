#pragma once

#include "../../inc/include.hpp"

#include "../Adapter.hpp"
#include "../Graphic/ReactionDiffusion/ReactionDiffusionSimulator.hpp"

class Adapter;
class ReactionDiffusionSimulator;

class HooksUI {
private:
	ReactionDiffusionSimulator& _RDSimulator;
	Adapter& _adapter;

	const std::map<std::string, glm::vec2>& _slidersRanges;

	void displayPropertieIndex(AdapterHook& hook, const int& i);
	glm::vec2 getSliderRangesFromHookPropertie(const int& index, const int& propertieIndex) const;

public:
	HooksUI(ReactionDiffusionSimulator& RDSimulator, Adapter& adapter, std::map<std::string, glm::vec2>& slidersRanges);

	void print();
};
