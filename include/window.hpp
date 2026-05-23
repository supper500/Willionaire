#pragma once

#include "ui.hpp"

#include <GLFW/glfw3.h>
#include <iostream>
using std::cin;
using std::cout;
using std::endl;

class Window{
	static constexpr unsigned FPS=60u;
	static int _x, _y;
public:
	Window()=delete;
	static GLFWwindow* window;
	static int init();
	static void show(){glfwShowWindow(window);}
	static void hide(){glfwHideWindow(window);}
	static void loop();
	static void operation(void(*operation)()=NULL);
	static void stopLoop();
	static int X(){return _x;}
	static int Y(){return _y;}
};