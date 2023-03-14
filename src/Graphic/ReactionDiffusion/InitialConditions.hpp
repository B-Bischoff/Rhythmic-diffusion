#pragma once

#include "../../../inc/include.hpp"
#include "../OpenGlHelper/ComputeShader.hpp"
#include "../OpenGlHelper/Texture.hpp"

enum Shape { Circle, Triangle, Hexagon };

struct InitialConditionsShape {
	float radius, borderSize, rotationAngle;
	Shape shape;

	InitialConditionsShape(const Shape shape, const float radius, const float borderSize, const float rotationAngle)
		: shape(shape), radius(radius), borderSize(borderSize), rotationAngle(rotationAngle) { }
};

class InitialConditions {
private:
	ComputeShader _computeShader;
	std::vector<ComputeShader> _shaders;

	float _radius, _bordersSize, _rotationAngle;
	std::vector<InitialConditionsShape> _shapes;

public:
	InitialConditions();
	InitialConditions(const std::string& computeShaderPath);

	void changeShader(const std::string& computeShaderPath);

	void execShader(const glm::vec2& screenDimensions);

	void setRadius(const float& radius);
	void setBordersSize(const float& radius);
	void setRotationAngle(const float& rotationAngle);
};
