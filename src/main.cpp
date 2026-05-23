#include "game.hpp"
#include "ui.hpp"
#include "window.hpp"
#include <winsock2.h>
#include <windows.h>
#include <ws2tcpip.h>

static bool init_winsock() {
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
		cout << "WSAStartup failed. Error:" << WSAGetLastError() << endl;
		return false;
	}
	return true;
}

int serverMain();

int main(){
	system("chcp 65001");
	assert(!Window::init());
	assert(init_winsock());
	srand(time(0));
	Game::init();
	UI::init();
	glCheckError();
	Game::state=Game::Menu;
	UI::update();
	Window::show();
	Window::loop();
	glfwTerminate();
	if(Game::state==Game::Server)
		serverMain();
	return 0;
}