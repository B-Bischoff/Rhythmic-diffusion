#include "GradientUI.hpp"

GradientUI::GradientUI(ReactionDiffusionSimulator& RDSimulator, ImGradient& gradient)
	: _RDSimulator(RDSimulator), _gradient(gradient)
{
	_draggingMark = new ImGradientMark();
	_selectedMark = new ImGradientMark();
}

void GradientUI::print()
{
	ImGui::Text("\n");
	bool updateGradient = false;
	if (ImGui::Button("Remove selected mark"))
	{
		_gradient.removeMark(_selectedMark);
		updateGradient = true;
	}
	if (ImGui::GradientEditor(&_gradient, _draggingMark, _selectedMark))
		updateGradient = true;

	if (!updateGradient)
		return;

	std::list<ImGradientMark*> markList = _gradient.getMarks();
	std::vector<glm::vec4> gradient;

	const int markListSize = (int)markList.size();
	for (int i = 0; i < markListSize; i++)
	{
		ImGradientMark* mark = markList.front();

		glm::vec4 colorAndPosition(mark->color[0], mark->color[1], mark->color[2], mark->position);
		gradient.push_back(colorAndPosition);

		markList.pop_front();
	}

	if (_gradient.getMarks().size() >= 10)
		_gradient.getMarks().pop_back();

	_RDSimulator.setPostProcessingGradient(gradient, 0.0);
}
