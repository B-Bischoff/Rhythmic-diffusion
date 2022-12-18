#include "Object.hpp"

Object::Object()
{
}

Object::Object(std::vector<float> positions, std::vector<float> textureCoords, std::vector<unsigned int> indices)
{
	InitPositions(positions);
	InitIndices(indices);
	InitTextureCoords(textureCoords);
	InitVao();
}

void Object::InitPositions(std::vector<float> positions)
{
	this->_positions = positions;

	glGenBuffers(1, &_positionBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, _positionBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * _positions.size(), &_positions[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Object::InitIndices(std::vector<unsigned int> indices)
{
	this->_indices = indices;

	glGenBuffers(1, &_ibo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * _indices.size(), &_indices[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void Object::InitTextureCoords(std::vector<float> textureCoords)
{
	this->_textureCoords = textureCoords;

	glGenBuffers(1, &_textureCoordsBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, _textureCoordsBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * textureCoords.size(), &_textureCoords[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Object::InitVao()
{
	glGenVertexArrays(1, &this->_vao);
	glBindVertexArray(this->_vao);

	glBindBuffer(GL_ARRAY_BUFFER, this->_positionBuffer);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);

	glBindBuffer(GL_ARRAY_BUFFER, this->_textureCoordsBuffer);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), 0);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->_ibo);

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glUseProgram(0);
}

void Object::render()
{
	glBindVertexArray(this->_vao);
	glDrawElements(GL_TRIANGLES, _indices.size(), GL_UNSIGNED_INT, 0);

	glBindVertexArray(0);
	glUseProgram(0);
}
