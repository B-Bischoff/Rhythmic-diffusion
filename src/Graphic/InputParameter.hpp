#pragma once

#include "../../inc/include.hpp"
#include "./ComputeShader.hpp"
#include "./Texture.hpp"

class ComputeShader;
class Texture;
class InputParameterSettings;

enum InputParameterType { Number, PerlinNoise, Voronoi };

class InputParameter {
private:
	InputParameterType _type;
	std::vector<float> _parameters;
	ComputeShader _computeShader;
	Texture* _parametersTexture;

	void applyParameterSettings();

	void applyNumberSettings();
	void applyPerlinNoiseSettings();
	void applyVoronoiSettings();


public:
	InputParameter();
	~InputParameter();
	InputParameter(Texture* texture);

	void changeType(const InputParameterType& newType);
	void changeType(const int& newTypeIndex);
	void execShader(const glm::vec4& channels, const glm::vec2& SCREEN_DIMENSION);
	std::vector<float>& getVectorParameters();
	const InputParameterType& getType() const;
};
