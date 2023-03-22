#pragma once

#include "../../../inc/include.hpp"
#include "../OpenGlHelper/ComputeShader.hpp"
#include "../OpenGlHelper/Texture.hpp"

class ComputeShader;
class Texture;
class InputParameterSettings;

enum ParameterName { DiffusionRateA, DiffusionRateB, FeedRate, KillRate };
enum InputParameterType { Number, PerlinNoise, Voronoi };

struct Parameter {
	std::vector<float> parameters;
	InputParameterType type;

	template <class Archive>
	void serialize(Archive& archive)
	{
		archive(CEREAL_NVP(parameters), CEREAL_NVP(type));
	}
};

class InputParameter {
private:
	InputParameterType _type;
	std::vector<Parameter> _parameters;
	ComputeShader _computeShader;
	Texture* _parametersTexture;
	bool _needToApplyChanges;

	void applyPerlinNoiseSettings(Parameter& parameter, const int parameterIndex);

public:
	InputParameter();
	~InputParameter();
	InputParameter(Texture* texture, const std::vector<std::string>& computeShadersPath);

	void changeType(const int& parameterIndex, const InputParameterType& newType);
	void changeType(const int& parameterIndex, const int& newTypeIndex);
	void setParameterValue(const int& parameterIndex, const std::vector<float>& parameterValues);
	void execShader(const glm::vec2& SCREEN_DIMENSION);
	std::vector<float>& getParameterValue(const int& index);
	std::vector<Parameter>& getParametersValue();
	InputParameterType getParameterType(const int& parameterIndex);
};
