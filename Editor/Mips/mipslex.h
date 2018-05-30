#ifndef MIPS_LEXEME_H
#define MIPS_LEXEME_H
#include <cstdio>
#include <iostream>
#include <vector>
#include <string>
#include "../Editor/lang.h"

#define _CRT_SECURE_NO_WARNINGS

using std::vector;
using std::string;

enum MIPSTOKENTYPE {
	MIPS_TT_USER,
	MIPS_TT_DATA,
	MIPS_TT_OP,
	MIPS_TT_SYS,
	MIPS_TT_REG,
};

enum MIPSIDENTIFIER {
	MIPS_ID_DADDR,
	MIPS_ID_CADDR,
	MIPS_ID_DB,
	MIPS_ID_DW,
	MIPS_ID_DD,
	MIPS_ID_EQU,
	MIPS_ID_LW,
	MIPS_ID_LB,
	MIPS_ID_LBU,
	MIPS_ID_LH,
	MIPS_ID_LHU,
	MIPS_ID_SB,
	MIPS_ID_SH,
	MIPS_ID_SW,
	MIPS_ID_ADD,
	MIPS_ID_ADDU,
	MIPS_ID_ADDS,
	MIPS_ID_SUB,
	MIPS_ID_SUBU,
	MIPS_ID_SLT,
	MIPS_ID_SLTU,
	MIPS_ID_AND,
	MIPS_ID_OR,
	MIPS_ID_XOR,
	MIPS_ID_NOR,
	MIPS_ID_SLL,
	MIPS_ID_SLLV,
	MIPS_ID_SRL,
	MIPS_ID_SRLV,
	MIPS_ID_SRA,
	MIPS_ID_SRAV,
	MIPS_ID_MULT,
	MIPS_ID_MULTU,
	MIPS_ID_MULTS,
	MIPS_ID_DIV,
	MIPS_ID_DIVU,
	MIPS_ID_ADDI,
	MIPS_ID_ADDIU,
	MIPS_ID_ANDI,
	MIPS_ID_ORI,
	MIPS_ID_XORI,
	MIPS_ID_LUI,
	MIPS_ID_SLTI,
	MIPS_ID_SLTIU,
	MIPS_ID_BEQ,
	MIPS_ID_BNE,
	MIPS_ID_BLEZ,
	MIPS_ID_BGTZ,
	MIPS_ID_BLTZ,
	MIPS_ID_BGEZ,
	MIPS_ID_J,
	MIPS_ID_JAL,
	MIPS_ID_JALR,
	MIPS_ID_JR,
	MIPS_ID_MFHI,
	MIPS_ID_MFLO,
	MIPS_ID_MTHI,
	MIPS_ID_MTLO,
	MIPS_ID_NOP
};
enum MIPSOPERATOR {
	MIPS_OP_PLUS,
	MIPS_OP_MINUS,
	MIPS_OP_NEG,
	MIPS_OP_MULTY,
	MIPS_OP_DIVIDE,
	MIPS_OP_MOD,
	MIPS_OP_AND,
	MIPS_OP_OR,
	MIPS_OP_NOR,
	MIPS_OP_INVERSE,
	MIPS_OP_LBRACE,
	MIPS_OP_RBRACE,
	MIPS_OP_LPARENTHESIS,
	MIPS_OP_RPARENTHESIS,
	MIPS_OP_LBRAKET,
	MIPS_OP_RBRAKET,
	MIPS_OP_SEMI,
	MIPS_OP_COMMA,
	MIPS_OP_DOT,
	MIPS_OP_NOT,
	MIPS_OP_COLON,
	MIPS_OP_QUOT,
	MIPS_OP_DBQUOT,
};

typedef struct _mipsHn {
	enum MIPSTOKENTYPE type;
	std::string name;
	int id;
	struct _mipsHn *next;
}mipsHashNode;
typedef struct {
	enum MIPSTOKENTYPE type;
	int id;
	float value;
	char *s = NULL;

	int line;
	int start, end;
}mipsTokenPrim;

class MipsLex : public Language {
private:
	string content;
	int idNum;

	mipsHashNode *list[256];

	int tmpLine = 0;
public:
	vector<mipsTokenPrim> output;
	vector<string> strId;

	MipsLex(const char *input = NULL);
	virtual ~MipsLex();

	int preserve(const char *str);
	void prepare();
	int hash(string s);

	MipsLex *input(const char *str);
	std::vector<mipsTokenPrim>parse();
	std::vector<std::pair<int, std::pair<int, int>>> highlight();
	std::vector<std::string> complete(std::string tab);

	const char *get();

	static void error(const char *word, int type);
};

enum MIPSLEXEMEERROR {
	MIPS_LE_ILLEGAL,
	MIPS_LE_TOOLONG,
	MIPS_LE_INCOMPLETE,
	MIPS_LE_EXPOP
};
class MIPSLexemeException {
private:
	std::string msg;
public:
	MIPSLexemeException(std::string s) {
		msg = s;
	}
	const char *message() { return msg.data(); }
};
#endif