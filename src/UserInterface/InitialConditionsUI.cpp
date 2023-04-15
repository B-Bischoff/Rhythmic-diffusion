#include "./InitialConditionsUI.hpp"

InitialConditionsUI::InitialConditionsUI(ReactionDiffusionSimulator& RDSimulator)
	: _RDSimulator(RDSimulator)
{
}

void InitialConditionsUI::print()
{
	ImGui::Text("\n");
	static int shape = 0;
	const char* shapesElements = "circle\0triangle\0hexagon\0square\0rectangle\0";
	ImGui::Combo("shape shape", &shape, shapesElements);

	static float radius = 0;
	ImGui::SliderFloat("shape radius", &radius, 0, 2000);

	static float borderSize = 0;
	ImGui::SliderFloat("shape border", &borderSize, 0, 1000);

	static float angle = 0;
	ImGui::SliderFloat("shape angle", &angle, 0, 360);

	float offset[2] = { 0, 0 };
	ImGui::SliderFloat2("shape offset", offset, -1000, 1000);

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
		if (ImGui::SliderFloat(str.c_str(), &shapeRadius, 0, 1000))
			shapes[i].radius = shapeRadius;

		float shapeBorder = shapes[i].borderSize;
		str = "border " + std::to_string(i);
		if (ImGui::SliderFloat(str.c_str(), &shapeBorder, 0, 1000))
			shapes[i].borderSize = shapeBorder;

		float shapeAngle = shapes[i].rotationAngle;
		str = "angle " + std::to_string(i);
		if (ImGui::SliderFloat(str.c_str(), &shapeAngle, 0, 360))
			shapes[i].rotationAngle = shapeAngle;

		float offset[2];
		offset[0] = shapes[i].offset.x;
		offset[1] = shapes[i].offset.y;
		str = "offset " + std::to_string(i);
		if (ImGui::SliderFloat2(str.c_str(), offset, -1000, 1000))
			shapes[i].offset = glm::vec2(offset[0], offset[1]);

		str = "erase shape " + std::to_string(i);
		if (ImGui::Button(str.c_str()))
		{
			_RDSimulator.removeInitialConditionsShape(i);
			return;
		}
	}
}
