#include "RDOptionsUI.hpp"

RDOptionsUI::RDOptionsUI(ReactionDiffusionSimulator& RDSimulator, const std::map<std::string, glm::vec2>& slidersRanges)
	: _RDSimulator(RDSimulator), _slidersRanges(slidersRanges)
{
}

void RDOptionsUI::print(const int& i)
{
	ImGui::Text("%s", getFieldNameFromIndex(i).c_str());
	ImGui::Text("\n");

	// Parameter preview
	bool showParam = _RDSimulator.getParameterPreview(i);
	if (ImGui::Checkbox(std::string("Show " + std::to_string(i)).c_str(), &showParam))
		_RDSimulator.setParameterPreview(i, showParam);

	ImGui::SameLine();
	// Input Type ComboBox
	int inputParameterType = _RDSimulator.getParameterType(i);
	ImGui::SetNextItemWidth(284);
	if (ImGui::Combo(std::string(getFieldNameFromIndex(i) + " type").c_str(), &inputParameterType, "number input\0Perlin noise\0Voronoi\0"))
		_RDSimulator.setParameterType(i, static_cast<InputParameterType>(inputParameterType));

	// Print input fields according to input type
	const InputParameterType& parameterType = _RDSimulator.getParameterType(i);
	if (parameterType == Number)
		printNumberTypeFields(i);
	else if (parameterType == PerlinNoise || parameterType == Voronoi)
		printNoiseFields(i);
}

void RDOptionsUI::printNumberTypeFields(const int& i)
{
	const glm::vec2 sliderRange = getSliderRangeFromOptionIndex(i);

	float value = sliderRange[0];
	std::vector<float> inputValue;
	try {
		inputValue = _RDSimulator.getParameterValue(i);
	}
	catch (std::out_of_range& e) {
		inputValue.push_back(0);
	}
	if (inputValue.size())
		value = inputValue[0];
	std::string fieldName = std::string("value " + std::to_string(i));
	if (ImGui::SliderFloat(fieldName.c_str(), &value, sliderRange[0], sliderRange[1],"%.5f"))
		_RDSimulator.setParameterValue(i, std::vector<float>(1, value));
}

void RDOptionsUI::printNoiseFields(const int& i)
{
	const glm::vec2 sliderRange = getSliderRangeFromOptionIndex(i);

	const std::vector<float>& v = _RDSimulator.getParameterValue(i);
	bool valueChanged = false;

	std::string fieldName;

	// Strength factor
	fieldName = std::string("Strength factor " + std::to_string(i));
	float strengthFactor = 1.0f;
	strengthFactor = v[0];
	if (ImGui::SliderFloat(fieldName.c_str(), &strengthFactor, sliderRange[0], sliderRange[1]))
		valueChanged = true;

	// Scale
	fieldName = std::string("scale " + std::to_string(i));
	float scale = v[1];
	if (scale < _slidersRanges.at("NoiseScale")[0])
		scale = _slidersRanges.at("NoiseScale")[0];
	if (ImGui::SliderFloat(fieldName.c_str(), &scale, _slidersRanges.at("NoiseScale")[0], _slidersRanges.at("NoiseScale")[1]))
		valueChanged = true;

	// Offset
	fieldName = std::string("offset " + std::to_string(i));
	float offset[2];
	offset[0] = v[2];
	offset[1] = v[3];
	if (ImGui::SliderFloat2(fieldName.c_str(), offset, _slidersRanges.at("NoiseOffset")[0], _slidersRanges.at("NoiseOffset")[1]))
		valueChanged = true;

	// Change scale depending on time
	fieldName = std::string("Scale moving " + std::to_string(i));
	bool movingScale = (bool)v[6];
	if (ImGui::Checkbox(fieldName.c_str(), &movingScale))
		valueChanged = true;

	// Time multiplier
	fieldName = std::string("Time multiplier" + std::to_string(i));
	float timeMultiplier = v[4];
	if (ImGui::SliderFloat(fieldName.c_str(), &timeMultiplier, 0.0, 1.0))
		valueChanged = true;

	// Base value
	fieldName = std::string("Base value" + std::to_string(i));
	float baseValue = v[5];
	if (ImGui::SliderFloat(fieldName.c_str(), &baseValue, sliderRange[0], sliderRange[1]))
		valueChanged = true;

	if (valueChanged)
	{
		std::vector<float> values = { strengthFactor, scale, offset[0], offset[1], timeMultiplier, baseValue, (movingScale ? 1.0f : 0.0f)};
		_RDSimulator.setParameterValue(i, values);
	}
}

std::string RDOptionsUI::getFieldNameFromIndex(const int& index) const
{
	switch (index)
	{
		case 0: return "Diffusion rate A";
		case 1: return "Diffusion rate B";
		case 2: return "Feed rate";
		case 3: return "Kill rate";
		default: return "";
	}
}

glm::vec2 RDOptionsUI::getSliderRangeFromOptionIndex(const int& optionIndex) const
{
	switch(optionIndex) {
		case 0 : return _slidersRanges.at("RDA");
		case 1 : return _slidersRanges.at("RDB");
		case 2 : return _slidersRanges.at("FeedRate");
		case 3 : return _slidersRanges.at("KillRate");
		default: return glm::vec2(0, 1);
	}
}
