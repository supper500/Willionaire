#include "player.hpp"
#include "career.hpp"
#include "card.hpp"
#include "buff.hpp"
#include "connection.hpp"
#include "game.hpp"

void Player::display(){
	sprintf(Connection::buf,"Player %d-display\n",id);
	Connection::flush();
	for(int i=0;i<5;i++){
		sprintf(Connection::buf,"%s:%d\n",Attri::name[i].c_str(),attri[i]);
		Connection::flush();
	}
	sprintf(Connection::buf,"energy:%d\n",energy);
	Connection::flush();
	sprintf(Connection::buf,"careers:%s %s %s\n",
		career[0]->name.c_str(),career[1]->name.c_str(),career[2]->name.c_str());
	Connection::flush();
	if(!buff.size()) return;
	sprintf(Connection::buf,"buffs:\n");
	Connection::flush();
	for(auto b:buff){
		sprintf(Connection::buf,"%s\n",b->name().c_str());
		Connection::flush();
	}
}
void Player::dice6(){
	sprintf(Connection::buf,"Player %d-dice6\n",id);
	Connection::flush();
	Connection::get(id,1,&dice);
	sprintf(Connection::buf,"-C %d\n",id);
	Connection::flush();
}
int Player::useEnergy(){
	int used=0;
	sprintf(Connection::buf,"Player %d-use energy(<=%d):\n",id,energy);
	Connection::flush();
	Connection::get(id,1,&used);
	sprintf(Connection::buf,"-N %d\n",id);
	Connection::flush();
	Connection::wait();
	Clamp(used,0,energy);
	for(auto b:buff) used=b->useEnergy(energy,used);
	energy-=used;
	return used;
}
void Player::chooseCareers(Career** c){
	sprintf(Connection::buf,"Player %d-choose 3 out of 5:\n",id);
	Connection::flush();
	for(int i=0;i<5;i++){
		sprintf(Connection::buf,"%d:%s\n",i,c[i]->name.c_str());
		Connection::flush();
	}
	#define I(t) Game::idMap[c[t]]
	sprintf(Connection::buf,"-E %d %d %d %d %d %d\n",id,I(0),I(1),I(2),I(3),I(4));
	#undef I
	Connection::flush();
}
Card* Player::chooseCards(Card** c){
	sprintf(Connection::buf,"Player %d-choose:\n",id);
	Connection::flush();
	sprintf(Connection::buf,"0:none\n");
	Connection::flush();
	for(int i=0;i<3;i++){
		sprintf(Connection::buf,"%d:%s%s\n",i+1,c[i]->name.c_str(),
			(c[i]->type==Card::Chance && c[i]->users.size())?"(unavailable)":"");
		Connection::flush();
	}
	int x;
	Connection::get(id,1,&x);
	sprintf(Connection::buf,"-A %d\n",id);
	Connection::flush();
	Connection::wait();
	Clamp(x,0,3);
	if(!x) return NULL;
	return c[x-1];
}
void Player::cardAttri(Attri a,Card* c){
	for(auto b:buff) a=b->cardEffect(a,c);
	attri+=a;
}
void Player::energyRecover(){
	int x=2;
	for(auto b:buff) x=b->energyRecover(x);
	energy+=x;
}
bool Player::win(){
	for(int i=0;i<3;i++)
		if(career[i]->win(*this)) return true;
	return false;
}