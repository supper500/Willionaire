#pragma once

#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

class Shader {
public:
	unsigned ID;
	Shader() :ID(0) {}
	void use() const { glUseProgram(ID); }
	void compile(const char* vertName, const char* fragName, const char* geomName = NULL);
	template<typename T>void set(const char* name, const T& value) const;
private:
	void checkError(unsigned object, int type, const char *name);
};