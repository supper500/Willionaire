#pragma once

#include <string>

class Player;
class Career{
public:
	static std::string title;
	std::string name;
	std::string description;
	typedef bool (*WIN)(const Player& p);
	WIN win;
	Career(std::string name,WIN win,std::string des):name(name),description(des),win(win){}
	std::string characters() const{return name+description;}
};