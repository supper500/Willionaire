#include "gl/texture.h"

#include "loader.h"

void Texture::init(const char* name) {
	GLFWimage image = Loader::loadGLFWimage(name);
	init(image.width, image.height, image.pixels);
}
void Texture::init(unsigned width, unsigned height, unsigned char* data){
	this->width = width, this->height = height;
	glGenTextures(1, &ID);
	bind();
	glTexImage2D(GL_TEXTURE_2D,0,format(),width,height,0,format(),GL_UNSIGNED_BYTE,data);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	bind(false);
}
void Texture::setBuf(unsigned char* data) const {
	bind();
	glTexImage2D(GL_TEXTURE_2D,0,format(),width,height,0,format(),GL_UNSIGNED_BYTE,data);
	bind(false);
}
void Texture::resize(unsigned width, unsigned height, unsigned char* data){
	glDeleteTextures(1, &ID);
	init(width,height,data);
}