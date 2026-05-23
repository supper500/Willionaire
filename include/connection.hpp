#pragma once

#include <map>
#include <vector>

class Connection{
	Connection(){}
	static bool satisfied;
	static std::map<unsigned, std::pair<unsigned, int*>> r;
public:
	static bool connected;
	static char buf[8192];
	static void wait();
	static void get(unsigned id, unsigned number, int* destination);
	static void get(const std::vector<std::pair<unsigned, int*>>& id);
	static void flush();
	static void input_web(unsigned id, const char* msg);
	static void output_web(const char* msg);
	static void back_web(const char* msg);
};