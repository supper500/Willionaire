#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "shader.h"
#include "texture.h"

GLenum glCheckError_(const char* file, int line);
#define glCheckError() glCheckError_(__FILE__, __LINE__) 

class Image:public Texture{
protected:
	const static float vb[16];
	glm::mat2 linear;
	glm::vec2 translation;
public:
	static unsigned VAO, VBO;
	static void initAll();
	static Shader shader;
	Image():Texture(),linear(1.0f),translation(0.0f){}
	void draw(Shader *shdr=&shader) const;
	void updateLinear(const glm::mat2 &x){linear=x;}
	void updateTranslation(const glm::vec2 &x){translation=x;}
	void destroy(){}
};

#if USE_TEST
class TestImage:public Image{
public:
	static Shader shader;
	TestImage():Image(){}
	void draw() const;
};
#endif