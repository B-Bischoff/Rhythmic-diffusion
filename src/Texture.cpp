#include "Texture.hpp"

Texture::Texture(const int& width, const int& height)
	: _width(width), _height(height)
{
	generateTexture();
	setTextureFiltering(GL_NEAREST, GL_NEAREST);
	setTextureWrapping(GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);
	glTextureStorage2D(this->_textureID, 1, GL_RGBA32F, width, height);
	glBindImageTexture(0, this->_textureID, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);
}

void Texture::generateTexture()
{
	glCreateTextures(GL_TEXTURE_2D, 1, &this->_textureID);
}

void Texture::setTextureFiltering(GLint sParam, GLint tParam) const
{
	glTextureParameteri(_textureID, GL_TEXTURE_MIN_FILTER, sParam);
	glTextureParameteri(_textureID, GL_TEXTURE_MAG_FILTER, tParam);
}

void Texture::setTextureWrapping(GLint sParam, GLint tParam) const
{
	glTextureParameteri(_textureID, GL_TEXTURE_WRAP_S, sParam);
	glTextureParameteri(_textureID, GL_TEXTURE_WRAP_T, tParam);
}

void Texture::useTexture(const int& GL_TEXTURE_NB) const
{
	glActiveTexture(GL_TEXTURE_NB);
	//glBindTexture(GL_TEXTURE_2D, this->_textureID);
	glBindTextureUnit(GL_TEXTURE_NB, this->_textureID);
}

unsigned int Texture::getTextureID() const
{
	return this->_textureID;
}

const int Texture::getWidth() const
{
	return this->_width;
}

const int Texture::getHeight() const
{
	return this->_height;
}
