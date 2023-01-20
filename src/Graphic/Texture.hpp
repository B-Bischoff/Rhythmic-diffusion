#pragma once

#include "../../inc/include.hpp"

class Texture {
private:

	unsigned int _textureID;
	int _width, _height, _nbrChannels;

	void generateTexture();
	void setTextureWrapping(GLint sParam, GLint tParam) const;
	void setTextureFiltering(GLint sParam, GLint tParam) const;

public:
	Texture();
	Texture(const int& width, const int& height);

	void useTexture(const int& GL_TEXTURE_NB) const;

	unsigned int getTextureID() const;
	const int getWidth() const;
	const int getHeight() const;
};
