#pragma once

#include <string>

#define Clamp(a,l,r) a=a<(l)?(l):a>(r)?(r):a
struct Attri{
	union{int intelligence,i;};
	union{int strength,s;};
	union{int charm,c;};
	union{int fortune,f;};
	union{int talent,t;};
	int offset;//for [5]
	const static std::string name[5];
	int& operator [](int id){return *((int*)this+id);}
	int operator [](int id) const{return *((int*)this+id);}
	bool zero() const{return !(i+s+c+f+t);}
	void check(){
		Clamp(i,1,10);
		Clamp(s,1,10);
		Clamp(c,1,10);
		Clamp(f,1,10);
		Clamp(t,1,10);
	}
	int sum() const{return i+s+c+f+t;}
	Attri(int x):i(x),s(x),c(x),f(x),t(x),offset(0){}
	Attri():i(0),s(0),c(0),f(0),t(0),offset(0){};
	Attri(int intelligence,int strength,int charm,
		int fortune,int talent):i(intelligence),
		s(strength),c(charm),f(fortune),t(talent),offset(0){}
	friend Attri operator +(const Attri& a,const Attri& b){
		return Attri(a.intelligence+b.intelligence,
			a.strength+b.strength,
			a.charm+b.charm,
			a.fortune+b.fortune,
			a.talent+b.talent);
	}
	friend Attri& operator +=(Attri& a,const Attri& b){
		a.intelligence+=b.intelligence;
		a.strength+=b.strength;
		a.charm+=b.charm;
		a.fortune+=b.fortune;
		a.talent+=b.talent;
		return a;
	}
	int maxi() const{
		int ans=0;
		bool repeat=false;
		for(int i=1;i<5;i++)
			if(operator[](i)>operator[](ans))
				ans=i,repeat=false;
			else if(operator[](i)==operator[](ans)) repeat=true;
		return repeat?5:ans;
	}
};