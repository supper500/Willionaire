#include "game.hpp"
#include "buff.hpp"

Game::State Game::state;
int Game::rounds;
std::vector<Player> Game::players;
std::vector<Buff*> Game::buffs;
std::vector<Card*> Game::cards;
std::vector<Career*> Game::careers;
std::map<void*,int> Game::idMap;
Card* Game::choice[3];

int read_s(const char* &s);
void Game::clear(){
	state=Uninited;
	for(auto &p:players)
		for(auto b:p.buff) delete b;
	players.clear();
	rounds=0;
	for(int i=0;i<3;i++) choice[i]=NULL;
}
void Game::read(const char* s){
	clear();
	state=(Game::State)read_s(s);
	rounds=read_s(s);
	int pn=read_s(s);
	for(int id=0;id<pn;id++){
		players.emplace_back(id);
		auto &p=players.back();
		for(int i=0;i<5;i++) p.attri[i]=read_s(s);
		p.energy=read_s(s);
		for(int n=read_s(s);n--;)
			p.buff.emplace_back(buffs[read_s(s)]->read(s));
		for(int n=read_s(s);n--;)
			p.history.emplace_back(cards[read_s(s)]);
		for(int i=0;i<3;i++)
			p.career[i]=careers[read_s(s)];
		p.color=read_s(s);
	}
	for(int i=0;i<3;i++)
		choice[i]=cards[read_s(s)];
}
void Game::write(char* s){
	s+=sprintf(s,"-G %d %d %llu",state,rounds,players.size());
	for(auto &p:players){
		for(int i=0;i<5;i++)
			s+=sprintf(s," %d",p.attri[i]);
		s+=sprintf(s," %d %llu",p.energy,p.buff.size());
		for(auto b:p.buff) s+=b->write(s);
		s+=sprintf(s," %llu",p.history.size());
		for(auto c:p.history) s+=sprintf(s," %d",idMap[c]);
		for(int i=0;i<3;i++) s+=sprintf(s," %d",idMap[p.career[i]]);
		s+=sprintf(s," %d",p.color);
	}
	for(int i=0;i<3;i++)
		s+=sprintf(s," %d",idMap[choice[i]]);
	*(s++)='\n';
	*s='\0';
}