#pragma once
#include "sgsmach.h"

void sgsDebug();

class SgsDebug : public SgsMachine {
public:
	void execute(int step);
	varNode *getVar(string name);
};
