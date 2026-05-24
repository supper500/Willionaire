#include "window.hpp"
#include "ui.hpp"
#include "connection.hpp"
#include <chrono>
#include <glad/glad.h>

GLFWwindow* Window::window;
int Window::_x, Window::_y;

int Window::init(){
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
	glfwWindowHint(GLFW_SAMPLES, 4);

	unsigned wndWidth = 1920, wndHeight = 1080;
	window = glfwCreateWindow(wndWidth, wndHeight, "Willionaire", NULL, NULL);
	if (!window) {
		std::cerr << "Window creation failed.\n";
		return -1;
	}
	glfwMakeContextCurrent(window);
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cerr << "GLAD initialization failed.\n";
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetInputMode(window, GLFW_STICKY_MOUSE_BUTTONS, GLFW_TRUE);
	glfwSetMouseButtonCallback(window, UI::click);
	glfwSetKeyCallback(window, UI::press);
	glfwSetFramebufferSizeCallback(window,
		[](GLFWwindow*, int width, int height){
			glViewport(0, 0, width, height);
			_x=width, _y=height;
		});
	glfwGetFramebufferSize(window, &_x, &_y);
	int width, height;
	glfwGetWindowSize(window, &width, &height);
	glfwSetWindowAspectRatio(window, width, height);
	return 0;
}
using namespace std::chrono;
using namespace std::chrono_literals;
#define Time() std::chrono::steady_clock::now()
static steady_clock::time_point tick[256],nedT;
static unsigned continousNoPaint,ticknum;
#if false
static auto averageDt(){
	if(tick[ticknum].time_since_epoch().count())
		return (tick[(ticknum-1)&255]-tick[ticknum])/255;
	return 0ns;
}
#endif
bool stop;
void (*_operation)();
void Window::loop(){
	auto nedT=Time();
	while(!glfwWindowShouldClose(window) && !stop){
		auto now=tick[ticknum++]=Time();
		ticknum&=255;
		if(now<nedT || continousNoPaint==5){
			continousNoPaint=0;
			while(Time()<nedT);
			UI::draw();
		}else continousNoPaint++;
		nedT+=duration_cast<nanoseconds>(1.0s/FPS);
		// if(averageDt()*(fps-1)>1s)
		// 	std::cout<<1.0s/averageDt()<<std::endl;
		glfwPollEvents();
		glfwSwapBuffers(window);
		void checkCommand();
		if(Connection::connected) checkCommand();
		if(_operation) _operation();
	}
	stop=false;
}
void Window::stopLoop(){stop=true;}
void Window::operation(void(*operation)()){_operation=operation;}