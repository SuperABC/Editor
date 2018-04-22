#pragma once
#include <vector>
#include "mipslex.h"

void mipsAssemble();

typedef struct _inst {
	MIPSIDENTIFIER op;
	int rd, rs, rt;
	int shamt, funct;
} instNode;

class MipsAsm {
private:
	vector<mipsTokenPrim> content;
	vector<std::pair<string, int>> label;
	vector<string> strId;

	unsigned int proc;
public:
	vector<instNode>output;

	MipsAsm();
	MipsAsm(vector<string> &ids, vector<mipsTokenPrim> &input);
	~MipsAsm();

	MipsAsm *input(vector<string> &ids, vector<mipsTokenPrim> &input);
	vector<instNode>parse();
	void write(string filename);
};
