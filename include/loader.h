#pragma once

#include <map>
#include <string>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#define USE_TEST false

class Loader{
public:
	static void init();
	static const unsigned char* load(std::string name) { return rawMap[name].first; }
	static GLFWimage loadGLFWimage(std::string);
	static char* loadString(std::string);
private:
	static std::map<std::string, std::pair<const unsigned char*, unsigned>> rawMap;
	static std::map<std::string, char*> stringMap;
	static std::map<std::string, GLFWimage> imageMap;
	Loader() {}
};