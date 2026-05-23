#pragma once

#include <vector>
#include "attri.hpp"

class Buff;
class Card;
class Career;

class Player{
public:
	int id;
	int priority;
	Attri attri;
	int energy;
	std::vector<Buff*> buff;
	std::vector<Card*> history;
	Career* career[3];
	int dice;
	int color;
	Player(int id):id(id),attri(1),energy(0),career{NULL},color(0){}
	bool operator <(const Player &b){
		if(priority!=b.priority)
			return priority<b.priority;
		return attri.intelligence<b.attri.intelligence;
	}
	void display();
	//call Connection::wait() to get input
	void dice6();
	int useEnergy();
	//only print the careers and don't actually choose the carrers
	void chooseCareers(Career** c);
	Card* chooseCards(Card** c);
	void cardAttri(Attri a,Card* c);
	void energyRecover();
	bool win();
	void check(){attri.check();Clamp(energy,0,10);}
};