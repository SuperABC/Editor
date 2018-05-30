#ifndef CPP_GCC_H
#define CPP_GCC_H

#include <iostream>
#include <string>
#include "pipe.h"

class Gcc {
private:
	std::string compiler;

	Pipe pipe;
public:
	Gcc(std::string path = "") { compiler = path; }
	std::string compile(std::string src);
	void run(std::string exe);
};

void cppRun();
void cppDebug();

#endif

