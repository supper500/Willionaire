#include "card.hpp"
#include "player.hpp"
#include "connection.hpp"
#include <windows.h>

std::array<std::string,5> Card::typeName;
std::string Card::title;

void Card::use(){
	Sleep(1000);
	for(auto p:users) p->history.emplace_back(this),p->check();
	users.clear();
}

void CardFeiWenHuaKe::use(){
	for(auto p:users){
		sprintf(Connection::buf,"Player %d-%s-choose the attribute to improve\n",
			p->id,name.c_str());
		Connection::flush();
		sprintf(Connection::buf,"0:%s+1\n",Attri::name[1].c_str());
		Connection::flush();
		sprintf(Connection::buf,"1:%s+1\n",Attri::name[4].c_str());
		Connection::flush();
		int x;
		Connection::get(p->id,1,&x);
		sprintf(Connection::buf,"-F %d\n",p->id);
		Connection::flush();
		Connection::wait();
		if(x) p->cardAttri(Attri(0,0,0,0,1),this);
		else p->cardAttri(Attri(0,1,0,0,0),this);
		p->check();
	}
	Card::use();
}

void CardPractice::use(){
	if(!users.size()) return;
	auto winner=users[0];
	for(auto p:users)
		p->priority=p->useEnergy();
	for(auto p:users)
		p->dice6();
	Connection::wait();
	for(auto p:users)
		if((p->priority+=Clamp(p->dice,1,6))>winner->priority)
			winner=p;
	winner->cardAttri(benefit,this);
	Card::use();
}

void CardChance::use(){
	if(!users.size()) return;
	auto p=users[0];
	sprintf(Connection::buf,"Player %d-%s\n",p->id,name.c_str());
	Connection::flush();
	p->dice6();
	Connection::wait();
	int x=Clamp(p->dice,1,6)+p->attri.fortune;
	if(x>=req) p->cardAttri(benefit,this);
	Card::use();
}