#include "gl/shader.h"

#include <string>
#include "loader.h"

using namespace glm;

enum { VERT, FRAG, GEOM, PROGRAM };

void Shader::compile(const char* vertName, const char* fragName, const char* geomName) {
	unsigned vertID = 0, fragID = 0, geomID = 0;

	char* vertS = Loader::loadString(vertName);
	vertID = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertID, 1, &vertS, NULL);
	glCompileShader(vertID);
	checkError(vertID, VERT, vertName);

	char* fragS = Loader::loadString(fragName);
	fragID = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragID, 1, &fragS, NULL);
	glCompileShader(fragID);
	checkError(fragID, FRAG, fragName);

	if (geomName) {
		char* geomS = Loader::loadString(geomName);
		geomID = glCreateShader(GL_GEOMETRY_SHADER);
		glShaderSource(geomID, 1, &geomS, NULL);
		glCompileShader(geomID);
		checkError(geomID, GEOM, geomName);
	}

	ID = glCreateProgram();
	glAttachShader(ID, vertID);
	glAttachShader(ID, fragID);
	if (geomName) glAttachShader(ID, geomID);
	glLinkProgram(ID);
	checkError(ID, PROGRAM, (std::string(vertName)+std::string(fragName)).c_str());

	glDeleteShader(vertID);
	glDeleteShader(fragID);
	if(geomName) glDeleteShader(geomID);
}
template<>
void Shader::set<int>(const char* name, const int& value) const {
	glUniform1i(glGetUniformLocation(ID, name), value);
}

#define SET_VEC(x) \
template<> \
void Shader::set<vec##x>(const char* name, const vec##x& value) const { \
	glUniform##x##fv(glGetUniformLocation(ID, name), 1, &value[0]); \
}
SET_VEC(2)
SET_VEC(3)
SET_VEC(4)
#undef SET_VEC

template<>
void Shader::set<mat2>(const char* name, const mat2& value) const {
	glUniformMatrix2fv(glGetUniformLocation(ID, name), 1, GL_FALSE, value_ptr(value));
}
template<>
void Shader::set<mat4>(const char* name, const mat4& value) const {
	glUniformMatrix4fv(glGetUniformLocation(ID, name), 1, GL_FALSE, value_ptr(value));
}

void Shader::checkError(unsigned object, int type, const char *name) {
	int success;
	char log[1024];
	if (type != PROGRAM) {
		glGetShaderiv(object, GL_COMPILE_STATUS, &success);
		if (!success) {
			glGetShaderInfoLog(object, 1024, NULL, log);
			std::cerr << name <<" shader compilation error,type:" << type << '\n' << log;
		}
	}
	else {
		glGetProgramiv(object, GL_LINK_STATUS, &success);
		if (!success) {
			glGetProgramInfoLog(object, 1024, NULL, log);
			std::cerr << name << " shader linking error\n" << log;
		}
	}
}