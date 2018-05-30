#pragma once
#include <vector>
#include "mipslex.h"

void mipsAssemble();

typedef struct _inst {
	MIPSIDENTIFIER op;
	unsigned int value = 0;
	unsigned int offset, length;

	int line;

	void setOpcode(int bin) {
		value &= 0x03FFFFFF;
		value |= ((bin & 0x3F) << 26);
	}
	void setRs(int bin) {
		value &= 0xFC1FFFFF;
		value |= ((bin & 0x1F) << 21);
	}
	void setRt(int bin) {
		value &= 0xFFE0FFFF;
		value |= ((bin & 0x1F) << 16);
	}
	void setRd(int bin) {
		value &= 0xFFFF07FF;
		value |= ((bin & 0x1F) << 11);
	}
	void setShamt(int bin) {
		value &= 0xFFFFF83F;
		value |= ((bin & 0x1F) << 6);
	}
	void setFunct(int bin) {
		value &= 0xFFFFFFC0;
		value |= (bin & 0x3F);
	}
	void setImm(int bin) {
		value &= 0xFFFF0000;
		value |= (bin & 0xFFFF);
	}
	void setTarget(int bin) {
		value &= 0xFC000000;
		value |= (bin & 0x3FFFFFF);
	}
	int operator[](string part) {
		if (part == "opcode")return value >> 26;
		else if (part == "rs")return (value >> 21) & 0x1F;
		else if (part == "rt")return (value >> 16) & 0x1F;
		else if (part == "rd")return (value >> 11) & 0x1F;
		else if (part == "shamt")return (value >> 6) & 0x1F;
		else if (part == "funct")return (value >> 0) & 0x3F;
		else if (part == "imm")return (value >> 0) & 0xFFFF;
		else if (part == "target")return (value >> 0) & 0x3FFFFFF;
		else return 0;
	}
} instNode;

enum expType {
	MIPS_VT_INTEGER,
	MIPS_VT_EXP,
	MIPS_VT_NULL
};
struct expNode {
	enum expType t;
	int val;
	expNode *left, *right;//Used for expression.
	expNode() : t(MIPS_VT_NULL), val(0), left(NULL), right(NULL) {};
};

class MipsAsm {
private:
	vector<mipsTokenPrim> content;
	vector<std::pair<string, int>> label;
	vector<string> strId;

	unsigned int proc;
	unsigned int dataAddr, codeAddr, tmpAddr;

	vector<std::pair<string, vector<std::pair<int, expNode *>>>> suspend;
public:
	vector<instNode>output;

	MipsAsm();
	MipsAsm(vector<string> &ids, vector<mipsTokenPrim> &input);
	~MipsAsm();

	MipsAsm *input(vector<string> &ids, vector<mipsTokenPrim> &input);
	vector<instNode>parse();
	void write(string filename);

	unsigned int parseLabel(int mode = -1);

	static void error(const char *inst, int type);
};

enum MIPSYNTAXERROR {
	MIPS_SE_UNIQUE,
	MIPS_SE_EXPCOLON,
	MIPS_SE_EXPCOMMA,
	MIPS_SE_EXPREG,
	MIPS_SE_EXPBRACE,
	MIPS_SE_REDEF,
	MIPS_SE_INVALIDTYPE,
	MIPS_SE_DISACCORD,
	MIPS_SE_NOID,
	MIPS_SE_INCOMPLETE,
	MIPS_SE_UNKNOWN,
	MIPS_SE_UNSUPPORT
};
class MIPSyntaxException {
private:
	std::string msg;
public:
	MIPSyntaxException(std::string s) {
		msg = s;
	}
	const char *message() { return msg.data(); }
};
