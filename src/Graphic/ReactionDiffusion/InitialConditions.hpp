#pragma once

#include "../../../inc/include.hpp"
#include "../OpenGlHelper/ComputeShader.hpp"
#include "../OpenGlHelper/Texture.hpp"

#define MAX_SHAPE 16 // The same value should be present in glslSpec.comp

enum Shape { None = -1, Circle, Triangle, Hexagon, Square, Rectangle};

struct InitialConditionsShape {
	Shape shape;
	float radius, borderSize, rotationAngle;
	glm::vec2 offset;

	InitialConditionsShape(const Shape shape, const float radius, const float borderSize, const float rotationAngle, const glm::vec2 offset)
		: shape(shape), radius(radius), borderSize(borderSize), rotationAngle(rotationAngle), offset(offset) { }
	InitialConditionsShape() {}

	template <class Archive>
	void serialize(Archive& archive)
	{
		archive(CEREAL_NVP(shape), CEREAL_NVP(radius), CEREAL_NVP(borderSize), CEREAL_NVP(rotationAngle), CEREAL_NVP(offset));
	}
};

class InitialConditions {
private:
	ComputeShader _computeShader;
	std::vector<InitialConditionsShape> _shapes;

public:
	InitialConditions();
	InitialConditions(const std::vector<std::string>& computeShadersPath);

	void execShader(const glm::vec2& screenDimensions);

	void addShape(InitialConditionsShape shape);
	std::vector<InitialConditionsShape>& getShapes();
	void clearShapes();
	void removeShape(const int& index);
};
