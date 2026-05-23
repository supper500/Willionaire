#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

class Texture {
private:
	GLenum format() const { return alpha ? GL_RGBA : GL_RGB; }
public:
	unsigned ID;
	unsigned width, height;
	bool alpha;
	Texture() :ID(0), width(0), height(0), alpha(true) {}
	void init(const char* name);
	void init(unsigned width, unsigned height, unsigned char* data);
	void bind(bool flag = true) const { glBindTexture(GL_TEXTURE_2D, flag ? ID : 0); }
	void setBuf(unsigned char* data) const;
	void resize(unsigned width, unsigned height, unsigned char* data);
};