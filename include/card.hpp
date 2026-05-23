#pragma once

#include <vector>
#include <string>
#include <utility>
#include <array>
#include "attri.hpp"
#include "player.hpp"

class Player;

class Card{
public:
	enum Type{
		Study,Practice,Chance,Rest,Competition
	};
	static std::array<std::string,5> typeName;
	static std::string title;
	Card(Type type,std::string name,std::string des):
		type(type),name(name),description(des){}
	Type type;
	std::string name;
	std::string description;
	virtual std::string characters() const{return name+description;}
	//remember to call it
	virtual void use();
	std::vector<Player*> users;
};


class CardStudy:public Card{
public:
	Attri benefit;
	CardStudy(std::string name,int intelligence,int strength,
		int charm,int fortune,int talent,std::string des):Card(Study,name,des),
		benefit(intelligence,strength,charm,fortune,talent){}
	virtual void use(){
		for(auto p:users) p->cardAttri(benefit,this);
		Card::use();
	}
};
class CardFeiWenHuaKe:public CardStudy{
public:
	CardFeiWenHuaKe(std::string name,std::string des):CardStudy(name,0,0,0,0,0,des){}
	virtual void use();
};
class CardPractice:public Card{
public:
	Attri benefit;
	CardPractice(std::string name,int intelligence,int strength,
		int charm,int fortune,int talent,std::string des):Card(Practice,name,des),
		benefit(intelligence,strength,charm,fortune,talent){}
	virtual void use();
};
class CardChance:public Card{
public:
	Attri benefit;
	int req;
	CardChance(std::string name,int intelligence,int strength,
		int charm,int fortune,int talent,int req,std::string des):Card(Chance,name,des),
		benefit(intelligence,strength,charm,fortune,talent),req(req){}
	virtual void use();
};
template<class B> class CardRest:public Card{
public:
	Attri benefit;
	CardRest(std::string name,int intelligence,int strength,
		int charm,int fortune,int talent,std::string des):Card(Rest,name,des),
		benefit(intelligence,strength,charm,fortune,talent){}
	virtual void use(){
		for(auto p:users){
			if(!benefit.zero()) p->cardAttri(benefit,this);
			p->buff.emplace_back(new B);
		}
		Card::use();
	}
};
class CardYouLiQiZhenHu:public Card{
public:
	CardYouLiQiZhenHu(std::string name,std::string des):Card(Rest,name,des){}
	virtual void use(){
		for(auto p:users){
			int h=p->attri[0],hn=1,l=p->attri[0],ln=1;
			for(int i=1;i<5;i++){
				if(p->attri[i]>h) h=p->attri[i],hn=1;
				else if(p->attri[i]==h) hn++;
				if(p->attri[i]<l) l=p->attri[i],ln=1;
				else if(p->attri[i]==l) ln++;
			}
			int hc=rand()%hn,lc=rand()%ln,hi=0,li=0;
			for(int i=0;i<5;i++){
				if(p->attri[i]==h) if(!(hc--)) hi=i;
				if(p->attri[i]==l) if(!(lc--)) li=i;
			}
			std::swap(p->attri[hi],p->attri[li]);
		}
		Card::use();
	}
};
class CardCompetition:public Card{
public:
	Attri benefit;
	int k1,k2;
	CardCompetition(std::string name,int intelligence,int strength,int charm,
		int fortune,int talent,int key1,int key2,std::string des):Card(Competition,name,des),
		benefit(intelligence,strength,charm,fortune,talent),k1(key1),k2(key2){}
	virtual void use(){
		if(!users.size()) return;
		auto winner=users[0];
		for(auto p:users)
			if(p->attri[k1]+p->attri[k2]>winner->attri[k1]+winner->attri[k2])
				winner=p;
		winner->cardAttri(benefit,this);
		Card::use();
	}
};