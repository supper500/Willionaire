#include "connection.hpp"

#include <cstdio>

int read_s(const char* &s){
	while((*s^48)>9 && *s) s++;
	if(!*s) return -1;
	int ans=0;
	while((*s^48)<10) ans=(ans<<3)+(ans<<1)+(*(s++)^48);
	return ans;
}

void Connection::input_web(unsigned id, const char* msg){
	if(msg[0]!='/') return;
	auto it=r.find(id);
	if(it==r.end()) return;
	auto arr=it->second;
	for(int v=read_s(msg);arr.first && v!=-1;v=read_s(msg)){
		*(arr.second++)=v;
		arr.first--;
	}
	if(!arr.first) r.erase(it);
	satisfied=r.empty();
}

bool Connection::satisfied;
std::map<unsigned, std::pair<unsigned, int*>> Connection::r;
char Connection::buf[8192];
bool Connection::connected;

void Connection::flush(){
	output_web(buf);
	printf("%s",buf);
}