#pragma once

#include "../inc/include.hpp"

class Object {
public:
	Object();
	Object(std::vector<float> positions,
		std::vector<float> textureCoords,
		std::vector<unsigned int> indices);

	void render();

private:
	std::vector<float> _positions, _textureCoords;
	std::vector<unsigned int> _indices;
	unsigned int _positionBuffer, _textureCoordsBuffer;
	unsigned int _vao, _ibo;

	void InitPositions(std::vector<float> positions);
	void InitIndices(std::vector<unsigned int> indices);
	void InitTextureCoords(std::vector<float> textureCoords);
	void InitVao();
};
