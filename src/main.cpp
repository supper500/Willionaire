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

void CreateConsole(){
    if(!AllocConsole()){
        MessageBoxA(NULL, "Failed to allocate console", "Error", MB_OK);
        return;
    }
    FILE* fDummy;
    freopen_s(&fDummy, "CONIN$", "r", stdin);
    freopen_s(&fDummy, "CONOUT$", "w", stdout);
    freopen_s(&fDummy, "CONOUT$", "w", stderr);

    std::cin.clear();
    std::cout.clear();
    std::cerr.clear();
    std::clog.clear();
}

int serverMain();

int main(){
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
	if(Game::state==Game::Server){
		CreateConsole();
		system("chcp 65001");
		serverMain();
	}
	return 0;
}