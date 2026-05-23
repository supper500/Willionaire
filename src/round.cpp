#include <algorithm>
#include "game.hpp"
#include "career.hpp"
#include "card.hpp"
#include "buff.hpp"
#include "connection.hpp"

void push(){
	Game::write(Connection::buf);
	Connection::flush();
}
bool Game::round(){
	sprintf(Connection::buf,"=====ROUND%d=====\n",++rounds);
	Connection::flush();
	for(auto &p:players){
		p.energyRecover();
		p.display();
	}
	state=ChooseCard;

	//choice
	Game::choice[0]=Game::choice[1]=Game::choice[2]=NULL;
	for(int i=0;i<3;i++){
		while(!choice[i]){
			choice[i]=cards[rand()%cards.size()];
			bool flag=false;
			for(int j=0;j<i;j++)
				flag|=choice[i]==choice[j];
			if(flag) choice[i]=NULL;
		}
	}
	sprintf(Connection::buf,"choices:%s %s %s\n",
		choice[0]->name.c_str(),choice[1]->name.c_str(),choice[2]->name.c_str());
	Connection::flush();
	push();
	sprintf(Connection::buf,"-D\n");
	Connection::flush();

	//sort
	for(auto &p:players){
		p.priority=0;
		for(auto &b:p.buff) p.priority+=b->prioSelect();
		if(p.priority>=0) p.priority+=p.useEnergy();
	}
	for(auto &p:players)
		p.dice6();
	Connection::wait();
	for(auto &p:players)
		p.priority+=Clamp(p.dice,1,6);
	std::sort(players.begin(),players.end());

	//choose
	for(auto &p:players){
		if(p.priority<0) break;
		auto c=p.chooseCards(choice);
		if(!c) continue;
		c->users.emplace_back(&p);
		push();
	}

	//update
	state=Update;
	push();
	for(int i=0;i<3;i++) choice[i]->use();
	for(auto &p:players){
		for(unsigned i=0;i<p.buff.size();)
			if(p.buff[i]->exist) i++;
			else{
				delete p.buff[i];
				p.buff.erase(p.buff.begin()+i);
			}
	}
	push();
	for(auto &p:players)
		if(p.win()){
			sprintf(Connection::buf,"Player %d wins\n",p.id);
			Connection::flush();
			sprintf(Connection::buf,"-W %d\n",p.id);
			Connection::flush();
			return true;
		}
	return false;
}