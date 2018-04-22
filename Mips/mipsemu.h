#pragma once
#include "mipsasm.h"

void mipsEmulate();

class MipsEmu : public MipsAsm{
private:
	long regs[32];
	char mem[4096];
public:
	MipsEmu();
	MipsEmu(vector<mipsTokenPrim> &input);
	~MipsEmu();

	void execute(int step);
	long getReg(int off);
	long getMem(int off);
};