#include "./InitialConditionsUI.hpp"

InitialConditionsUI::InitialConditionsUI(ReactionDiffusionSimulator& RDSimulator, const std::map<std::string, glm::vec2>& slidersRanges)
	: _RDSimulator(RDSimulator), _slidersRanges(slidersRanges)
{
}

void InitialConditionsUI::print()
{
	ImGui::Text("\n");
	static int shape = 0;
	const char* shapesElements = "circle\0triangle\0hexagon\0square\0rectangle\0";
	ImGui::Combo("shape shape", &shape, shapesElements);

	static float radius = 0;
	ImGui::SliderFloat("shape radius", &radius, _slidersRanges.at("ShapesRadius")[0], _slidersRanges.at("ShapesRadius")[1]);

	static float borderSize = 0;
	ImGui::SliderFloat("shape border", &borderSize, _slidersRanges.at("ShapesBorder")[0], _slidersRanges.at("ShapesBorder")[1]);

	static float angle = 0;
	ImGui::SliderFloat("shape angle", &angle, _slidersRanges.at("ShapesAngle")[0], _slidersRanges.at("ShapesAngle")[1]);

	static float offset[2] = { 0, 0 };
	ImGui::SliderFloat2("shape offset", offset, _slidersRanges.at("ShapesOffset")[0], _slidersRanges.at("ShapesOffset")[1]);

	if (ImGui::Button("add shape"))
		_RDSimulator.addInitialConditionsShape(InitialConditionsShape((Shape)shape, radius, borderSize, angle, glm::vec2(offset[0], offset[1])));

	ImGui::Text("\n");
	ImGui::Separator();
	ImGui::Text("\n");

	// Print existing shapes
	std::vector<InitialConditionsShape>& shapes = _RDSimulator.getInitialConditionsShapes();

	for (int i = 0; i < (int)shapes.size(); i++)
	{
		ImGui::Text("shape: %d", i);
		std::string str;

		int shapeShape = (int)shapes[i].shape;
		str = "shape " + std::to_string(i);
		if (ImGui::Combo(str.c_str(), &shapeShape, shapesElements))
			shapes[i].shape = (Shape)shapeShape;

		float shapeRadius = shapes[i].radius;
		str = "radius " + std::to_string(i);
		if (ImGui::SliderFloat(str.c_str(), &shapeRadius, _slidersRanges.at("ShapesRadius")[0], _slidersRanges.at("ShapesRadius")[1]))
			shapes[i].radius = shapeRadius;

		float shapeBorder = shapes[i].borderSize;
		str = "border " + std::to_string(i);
		if (ImGui::SliderFloat(str.c_str(), &shapeBorder, _slidersRanges.at("ShapesBorder")[0], _slidersRanges.at("ShapesBorder")[1]))
			shapes[i].borderSize = shapeBorder;

		float shapeAngle = shapes[i].rotationAngle;
		str = "angle " + std::to_string(i);
		if (ImGui::SliderFloat(str.c_str(), &shapeAngle, _slidersRanges.at("ShapesAngle")[0], _slidersRanges.at("ShapesAngle")[1]))
			shapes[i].rotationAngle = shapeAngle;

		float offset[2];
		offset[0] = shapes[i].offset.x;
		offset[1] = shapes[i].offset.y;
		str = "offset " + std::to_string(i);
		if (ImGui::SliderFloat2(str.c_str(), offset, _slidersRanges.at("ShapesOffset")[0], _slidersRanges.at("ShapesOffset")[1]))
			shapes[i].offset = glm::vec2(offset[0], offset[1]);

		str = "erase shape " + std::to_string(i);
		if (ImGui::Button(str.c_str()))
		{
			_RDSimulator.removeInitialConditionsShape(i);
			return;
		}
	}
}
