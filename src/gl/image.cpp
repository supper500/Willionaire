#include "gl/image.h"

GLenum glCheckError_(const char* file, int line) {
	GLenum errorCode;
	while ((errorCode = glGetError()) != GL_NO_ERROR) {
		std::string error;
		switch (errorCode) {
		case GL_INVALID_ENUM:                  error="INVALID_ENUM"; break;
		case GL_INVALID_VALUE:                 error="INVALID_VALUE"; break;
		case GL_INVALID_OPERATION:             error="INVALID_OPERATION"; break;
		case GL_STACK_OVERFLOW:                error="STACK_OVERFLOW"; break;
		case GL_STACK_UNDERFLOW:               error="STACK_UNDERFLOW"; break;
		case GL_OUT_OF_MEMORY:                 error="OUT_OF_MEMORY"; break;
		case GL_INVALID_FRAMEBUFFER_OPERATION: error="INVALID_FRAMEBUFFER_OPERATION"; break;
		}
		std::cerr << file << " (" << line << ") " << error << '\n';
	}
	return errorCode;
}

const float Image::vb[16] = {
	1.0f, 1.0f, 1.0f, 1.0f,
	1.0f, -1.0f, 1.0f, 0.0f,
	-1.0f, -1.0f, 0.0f, 0.0f,
	-1.0f, 1.0f, 0.0f, 1.0f
};
Shader Image::shader;
unsigned Image::VAO, Image::VBO;
void Image::initAll(){
	Image::shader.compile("CopyVert", "CopyFrag");
	shader.use();
	shader.set("Texture", 0);
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vb), vb, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, (void*)0);
}
void Image::draw(Shader *shdr) const{
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D,Texture::ID);
	shdr->use();
	shdr->set("linear",linear);
	shdr->set("translation",translation);
	glBindVertexArray(VAO);
	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
}

#if USE_TEST
Shader TestImage::shader;
void TestImage::draw() const{
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D,Texture::ID);
	shader.use();
	glBindVertexArray(VAO);
	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
}
#endif