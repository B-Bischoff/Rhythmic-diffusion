#pragma once

#include "../../../inc/include.hpp"
#include "../OpenGlHelper/ComputeShader.hpp"
#include "../OpenGlHelper/Texture.hpp"

#define MAX_SHAPE 16 // The same value should be present in glslSpec.comp

enum Shape { None = -1, Circle, Triangle, Hexagon };

struct InitialConditionsShape {
	Shape shape;
	float radius, borderSize, rotationAngle;

	InitialConditionsShape(const Shape shape, const float radius, const float borderSize, const float rotationAngle)
		: shape(shape), radius(radius), borderSize(borderSize), rotationAngle(rotationAngle) { }
};

class InitialConditions {
private:
	ComputeShader _computeShader;
	std::vector<ComputeShader> _shaders;

	std::vector<InitialConditionsShape> _shapes;


public:
	InitialConditions();
	InitialConditions(const std::string& computeShaderPath);

	void changeShader(const std::string& computeShaderPath);

	void execShader(const glm::vec2& screenDimensions);

	void addShape(InitialConditionsShape shape);
	std::vector<InitialConditionsShape>& getShapes();
	void clearShapes();
	void removeShape(const int& index);
};
