#pragma once

#include <map>
#include <vector>
#include "player.hpp"

class Game{
private:
	Game();
public:
	enum State{
		Uninited,
		ChooseCareer,
		ChooseCard,
		Update,
		Server,
		Client,
		Menu
	};
	static State state;
	static int rounds;
	static std::vector<Player> players;
	static Card* choice[3];
	static std::vector<Buff*> buffs;
	static std::vector<Card*> cards;
	static std::vector<Career*> careers;
	static std::map<void*,int> idMap;
	static void init();
	static bool round();
	static void clear();
	static void read(const char* s);
	static void write(char* s);
};