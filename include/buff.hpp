#pragma once

#include "attri.hpp"
#include "card.hpp"

class Buff{
public:
	int id;
	bool exist;
	Buff():id(1726),exist(true){}
	virtual std::string characters() const{return name()+description();}
	virtual std::string description() const{return "";}
	virtual std::string name() const{return "";}
	virtual int prioSelect(){return 0;}
	virtual Attri cardEffect(Attri a,Card* c){return a;}
	virtual int energyRecover(int x){return x;}
	virtual int useEnergy(int origin,int used){return used;}
	virtual Buff* read(const char* &s){return new Buff;}
	virtual int write(char* s){return sprintf(s," %d",id);}
	virtual ~Buff(){}
};
class BuffDouble:public Buff{
public:
	BuffDouble(){id=0;}
	virtual std::string name() const;
	virtual std::string description() const;
	virtual Attri cardEffect(Attri a,Card* c){exist=false;return a+a;}
	virtual Buff* read(const char* &s){return new BuffDouble;}
	virtual ~BuffDouble(){}
};
class BuffCoffee:public Buff{
public:
	BuffCoffee(){id=1;}
	virtual std::string name() const;
	virtual std::string description() const;
	virtual Attri cardEffect(Attri a,Card* c){
		if(c->type==Card::Study) a.intelligence++;
		return a;
	}
	virtual Buff* read(const char* &s){return new BuffCoffee;}
	virtual ~BuffCoffee(){}
};
class BuffPrio:public Buff{
public:
	BuffPrio(){id=2;}
	virtual std::string name() const;
	virtual std::string description() const;
	virtual int prioSelect(){exist=false;return 1000;}
	virtual Buff* read(const char* &s){return new BuffPrio;}
	virtual ~BuffPrio(){}
};
class BuffSleep:public Buff{
public:
	virtual std::string name() const;
	virtual std::string description() const;
	int turn;
	BuffSleep():turn(2){id=3;}
	BuffSleep(int turn):turn(turn){id=3;}
	virtual int prioSelect(){exist=!!--turn;return -1000000;}
	virtual Buff* read(const char* &s){
		int read_s(const char* &s);
		return new BuffSleep(read_s(s));
	}
	virtual int write(char* s){return sprintf(s," %d %d",id,turn);}
	virtual ~BuffSleep(){}
};
class BuffDaydream:public Buff{
public:
	BuffDaydream(){id=4;}
	virtual std::string name() const;
	virtual std::string description() const;
	virtual Attri cardEffect(Attri a,Card* c){a.intelligence=0;return a;}
	virtual int energyRecover(int x){return x?3:0;}
	virtual Buff* read(const char* &s){return new BuffDaydream;}
	virtual ~BuffDaydream(){}
};
class BuffLieFlat:public Buff{
public:
	BuffLieFlat(){id=5;}
	virtual std::string name() const;
	virtual std::string description() const;
	virtual int energyRecover(int x){return 0;}
	virtual int useEnergy(int origin,int used){return origin-6;}
	virtual Buff* read(const char* &s){return new BuffLieFlat;}
	virtual ~BuffLieFlat(){}
};