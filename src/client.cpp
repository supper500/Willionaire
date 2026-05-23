#include <string>
#include <chrono>
#include <queue>
#define Time() std::chrono::steady_clock::now()
using namespace std::chrono;
using namespace std::chrono_literals;
#include <winsock2.h>
#include <windows.h>
#include <iostream>
using std::cin;
using std::cout;
using std::endl;
#include "ui.hpp"

#define BUFFER_SIZE 65536

extern GLFWwindow* window;
static SOCKET sock;
static CRITICAL_SECTION cs;
static int thisID;
static std::queue<std::string> command;
int read_s(const char* &s);

static int init_winsock(){
	sock=socket(AF_INET,SOCK_STREAM,0);
	if(sock==INVALID_SOCKET){
		cout<<"socket failed. Error:"<<WSAGetLastError()<<endl;
		return 0;
	}
	return 1;
}

#include "connection.hpp"
void Connection::back_web(const char* msg){
	int sent=send(sock,msg,strlen(msg),0);
	if(sent==SOCKET_ERROR)
		cout<<"send failed. Error:"<<WSAGetLastError()<<endl;
}

void checkCommand(){
	EnterCriticalSection(&cs);
	while(command.size()){
		const char *tmp=command.front().c_str();
		tmp++;
		switch (*tmp) {
			case 'I' :{
				thisID=read_s(tmp);
				break;
			}
			case 'G' :{
				bool uni=(Game::state==Game::Uninited);
				Game::read(tmp);
				UI::update();
				if(uni) UI::start(thisID);
				break;
			}
			case 'N' :{
				int id=read_s(tmp);
				if (id==thisID) UI::requireEnergy();
				break;
			}
			case 'E' :{
				int id=read_s(tmp);
				if (id!=thisID) break;
				UI::addCareer(tmp);
				break;
			}
			case 'D' :{
				UI::displayCard();
				break;
			}
			case 'A' :{
				int id=read_s(tmp);
				if (id!=thisID) break;
				UI::chooseCard();
				break;
			}
			case 'F' :{
				int id=read_s(tmp);
				if (id!=thisID) break;
				UI::FWHK();
				break;
			}
			case 'H' :{
				int id=read_s(tmp);
				if (id!=thisID) break;
				if (Game::state==Game::Uninited){
					Connection::back_web("/0\n");
					break;
				}
				char buf[30];
				sprintf(buf,"/%d",UI::selfColor()<<1|1);
				Connection::back_web(buf);
				break;
			}
			case 'C' :{
				int id=read_s(tmp);
				if (id!=thisID) break;
				UI::dice();
				break;
			}
			case 'W' :{
				break;
			}
		}
		command.pop();
	}
	LeaveCriticalSection(&cs);
}

static DWORD WINAPI receive_thread(LPVOID param){
	SOCKET sock=(SOCKET)param;
	static char buffer[BUFFER_SIZE];
	static int recv_len;
	while((recv_len=recv(sock,buffer,BUFFER_SIZE-1,0))>0){
		buffer[recv_len]='\0';
		cout<<buffer;
		EnterCriticalSection(&cs);
		for(char *p=buffer,*q=p;*p!='\0';p=q){
			while(*q!='\n') q++;
			q++;
			if(*p=='-')
				command.emplace(p, q - p);
		}
		LeaveCriticalSection(&cs);
	}
	cout<<"Disconnected from server."<<endl;
	return 0;
}

#define RT() {closesocket(sock);return false;}
bool clientInit(const std::string& addr){
	auto sz=addr.find_first_of(':');
	if(sz==std::string::npos) return false;
	std::string ip{addr,0,sz};
	int port=0;
	if(!sscanf(addr.c_str()+sz+1,"%d",&port)) return false;
	if(port<=0 || port>65535) return false;

	struct sockaddr_in server_addr;
	server_addr.sin_family=AF_INET;
	server_addr.sin_port=htons(port);
	server_addr.sin_addr.s_addr=inet_addr(ip.c_str());
	if(server_addr.sin_addr.s_addr==INADDR_NONE) return false;
	if(!init_winsock()) return false;
	if(connect(sock,(struct sockaddr*)&server_addr,sizeof(server_addr))==SOCKET_ERROR) RT()
	cout<<"Connected."<<endl;

	InitializeCriticalSection(&cs);
	HANDLE hThread=CreateThread(NULL,0,receive_thread,(LPVOID)sock,0,NULL);
	if(!hThread) RT()
	CloseHandle(hThread);
	cout<<"Thread created."<<endl;

	thisID=-1;
	Connection::connected=true;
	// HWND hWnd=GetConsoleWindow();
	// ShowWindow(hWnd,SW_HIDE);
	return true;
}

void clientClose(){
	glfwTerminate();
	DeleteCriticalSection(&cs);
	closesocket(sock);
	WSACleanup();
}