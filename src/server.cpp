#include <cstring>
#include <map>
#include <winsock2.h>
#include <windows.h>
#include <ws2tcpip.h>
#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "game.hpp"
#include "connection.hpp"
using std::cin;
using std::cout;
using std::endl;

#define BUFFER_SIZE 8192
#define MAX_CLIENTS 64

static SOCKET client_sockets[MAX_CLIENTS];
static int client_count = 0;
static CRITICAL_SECTION cs;

static void broadcast_message(const char *msg, SOCKET sender) {
	EnterCriticalSection(&cs);
	for (int i = 0; i < client_count; i++) {
		SOCKET sock = client_sockets[i];
		if (sock != sender) {
			send(sock, msg, strlen(msg), 0);
		}
	}
	LeaveCriticalSection(&cs);
}

std::map<SOCKET,int> playerID;
void Connection::output_web(const char* msg){
	broadcast_message(msg, (SOCKET)NULL);
}
void Connection::wait(){
	LeaveCriticalSection(&cs);
	while(true){
		Sleep(100);
		EnterCriticalSection(&cs);
		if(satisfied) break;
		LeaveCriticalSection(&cs);
	}
	LeaveCriticalSection(&cs);
}
void Connection::get(unsigned id, unsigned number, int *destination){
	EnterCriticalSection(&cs);
	satisfied=false;
	r[id]={number,destination};
}
void Connection::get(const std::vector<std::pair<unsigned, int*>>& id){
	EnterCriticalSection(&cs);
	satisfied=false;
	for(const auto& i:id)
		r[i.first]={1,i.second};
	LeaveCriticalSection(&cs);
}

static DWORD WINAPI client_handler(LPVOID param) {
	SOCKET client_sock = (SOCKET)param;
	char buffer[BUFFER_SIZE];
	int recv_len;
	struct sockaddr_in client_addr;
	int addr_len = sizeof(client_addr);
	char client_ip[INET_ADDRSTRLEN];

	if (getpeername(client_sock, (struct sockaddr*)&client_addr, &addr_len) == 0) {
		strcpy(client_ip, inet_ntoa(client_addr.sin_addr));
	} else {
		strcpy(client_ip, "unknown");
	}

	while ((recv_len = recv(client_sock, buffer, BUFFER_SIZE - 1, 0)) > 0) {
		buffer[recv_len] = '\0';
		if (recv_len > 0 && buffer[recv_len-1] == '\n')
			buffer[recv_len-1] = '\0';

		char msg_with_ip[BUFFER_SIZE + INET_ADDRSTRLEN];
		snprintf(msg_with_ip, sizeof(msg_with_ip), "[%s] %s\n", client_ip, buffer);
		cout << msg_with_ip;
		broadcast_message(msg_with_ip, client_sock);
		EnterCriticalSection(&cs);
		Connection::input_web(playerID[client_sock],buffer);
		LeaveCriticalSection(&cs);
	}

	EnterCriticalSection(&cs);
	for (int i = 0; i < client_count; i++) {
		if (client_sockets[i] == client_sock) {
			client_sockets[i] = client_sockets[client_count - 1];
			client_count--;
			break;
		}
	}
	LeaveCriticalSection(&cs);

	closesocket(client_sock);
	cout << "Client [" << client_ip << "] disconnected." << endl;
	return 0;
}

int serverMain() {
	int n;
	cout << "Player number:";
	cin >> n;
	if (n <= 0 || n >= MAX_CLIENTS) {
		cout << "Invalid number." << endl;
		return 1;
	}

	int port;
	cout << "Port:";
	cin >> port;
	if (port <= 0 || port > 65535) {
		cout << "Invalid port." << endl;
		return 1;
	}
	SOCKET server_sock, client_sock;
	struct sockaddr_in server_addr, client_addr;
	int addr_len = sizeof(client_addr);

	server_sock = socket(AF_INET, SOCK_STREAM, 0);
	if (server_sock == INVALID_SOCKET) {
		cout << "socket failed. Error:" << WSAGetLastError() << endl;
		return 1;
	}


	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = INADDR_ANY;
	server_addr.sin_port = htons(port);

	if (bind(server_sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) == SOCKET_ERROR) {
		cout << "bind failed on port "<< port << ". Error:" << WSAGetLastError() << endl;
		closesocket(server_sock);
		return 1;
	}

	if (listen(server_sock, SOMAXCONN) == SOCKET_ERROR) {
		cout << "listen failed. Error:" << WSAGetLastError() << endl;
		closesocket(server_sock);
		return 1;
	}

	cout << "Server started on port " << port << ". Waiting for clients..." << endl;
	InitializeCriticalSection(&cs);

	while (client_count < n) {
		client_sock = accept(server_sock, (struct sockaddr*)&client_addr, &addr_len);
		if (client_sock == INVALID_SOCKET) {
			cout << "accept failed. Error:" << WSAGetLastError() << endl;
			continue;
		}

		char client_ip[INET_ADDRSTRLEN];
		strcpy(client_ip, inet_ntoa(client_addr.sin_addr));
		cout << "New client connected: " <<
			client_ip << ":" << ntohs(client_addr.sin_port) << endl;

		EnterCriticalSection(&cs);
		if (client_count < MAX_CLIENTS) {
			client_sockets[client_count++] = client_sock;
			HANDLE hThread = CreateThread(NULL, 0, client_handler, (LPVOID)client_sock, 0, NULL);
			if (hThread == NULL) {
				cout << "CreateThread failed." << endl;
				client_count--;
				closesocket(client_sock);
			} else {
				CloseHandle(hThread);
				playerID[client_sock] = client_count - 1;
				char buf[10];
				int len = sprintf(buf, "-I %d\n", client_count - 1);
				send(client_sockets[client_count - 1], buf, len, 0);
			}
		} else {
			cout << "Max clients reached. Connection refused." << endl;
			closesocket(client_sock);
		}
		LeaveCriticalSection(&cs);
	}

	cout << "Enough players." << endl;
	for(int i=0;i<n;i++)
		Game::players.emplace_back(i);
	Game::state=Game::ChooseCareer;
	void push();
	push();
	for(int i=0;i<n;i++){
		int ret=0;
		while(!ret){
			Connection::get(i,1,&ret);
			sprintf(Connection::buf,"-H %d\n",i);
			Connection::flush();
			Connection::wait();
			if(!ret) Sleep(200);
		}
		Game::players[i].color=ret>>1;
	}
	push();

	Career* (*choice)[5]=new Career*[n][5];
	int (*res)[3]=new int[n][3];
	for(int i=0;i<n;i++){
		Connection::get(i,3,res[i]);
		for(int j=0;j<5;j++){
			for(choice[i][j]=NULL;!choice[i][j];){
				choice[i][j]=Game::careers[rand()%Game::careers.size()];
				bool flag=false;
				for(int k=0;k<j;k++)
					flag|=choice[i][j]==choice[i][k];
				if(flag) choice[i][j]=NULL;
			}
		}
		Game::players[i].chooseCareers(choice[i]);
	}
	Connection::wait();
	for(int i=0;i<n;i++)
		for(int j=0;j<3;j++)
			Game::players[i].career[j]=choice[i][Clamp(res[i][j],0,4)];
	delete choice;
	delete res;

	while(!Game::round());

	DeleteCriticalSection(&cs);
	closesocket(server_sock);
	return 0;
}