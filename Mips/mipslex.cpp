#define _CRT_SECURE_NO_WARNINGS
#include "mipslex.h"

MipsLex::MipsLex(const char *input) {
	for (int i = 0; i < 256; i++) {
		list[i] = NULL;
	}
	idNum = 0;
	prepare();

	if (input == NULL)return;
	content = std::string(input);
}
MipsLex::~MipsLex() {
	int i = 0, ids = 256;
	while (ids--) {
		mipsHashNode *tmp = list[i];
		while (tmp) {
			list[i] = list[i]->next;
			delete tmp;
			tmp = list[i];
		}
		i++;
	}
	for (unsigned int i = 0; i < output.size(); i++) {
		if (output[i].s)delete output[i].s;
	}
}

int MipsLex::preserve(const char *str) {
	int value = 0;
	int len = strlen(str);
	for (int i = 0; i < len; i++) {
		value += str[i];
	}
	value %= 256;
	mipsHashNode *tmp = list[value];
	while (tmp != NULL && strcmp(tmp->name.c_str(), str) != 0) {
		tmp = tmp->next;
	}
	if (tmp == NULL) {
		tmp = new mipsHashNode();
		tmp->id = idNum++;
		tmp->type = MIPS_TT_SYS;
		tmp->name = string(str);
		tmp->next = list[value];
		list[value] = tmp;
		strId.push_back(string(str));
	}
	return tmp->id;
}
void MipsLex::prepare() {
	preserve("BaseAddre");
	preserve("DataAddre");
	preserve("db");
	preserve("dw");
	preserve("dd");
	preserve("equ");
	preserve("lw");
	preserve("lb");
	preserve("lbu");
	preserve("lh");
	preserve("lhu");
	preserve("sb");
	preserve("sh");
	preserve("sw");
	preserve("add");
	preserve("addu");
	preserve("sub");
	preserve("subu");
	preserve("slt");
	preserve("sltu");
	preserve("and");
	preserve("or");
	preserve("xor");
	preserve("nor");
	preserve("sll");
	preserve("srl");
	preserve("sra");
	preserve("mult");
	preserve("multu");
	preserve("div");
	preserve("divu");
	preserve("addi");
	preserve("addiu");
	preserve("andi");
	preserve("ori");
	preserve("xori");
	preserve("lui");
	preserve("slti");
	preserve("sltiu");
	preserve("beq");
	preserve("bne");
	preserve("blez");
	preserve("bgtz");
	preserve("bltz");
	preserve("bgez");
	preserve("j");
	preserve("jal");
	preserve("jalr");
	preserve("jr");
}
int MipsLex::hash(string str) {
	int value = 0;
	int len = str.length();
	for (int i = 0; i < len; i++) {
		value += str[i];
	}
	value %= 256;
	mipsHashNode *tmp = list[value];
	while (tmp != NULL && str != tmp->name) {
		tmp = tmp->next;
	}
	if (tmp == NULL) {
		tmp = new mipsHashNode();
		tmp->id = idNum++;
		tmp->type = MIPS_TT_USER;
		tmp->name = std::string(str);
		tmp->next = list[value];
		list[value] = tmp;
		strId.push_back(str);
	}
	return tmp->id;
}

MipsLex *MipsLex::input(const char *str) {
	content = std::string(str);
	return this;
}
vector<mipsTokenPrim> MipsLex::parse() {
	output.clear();

	int len = content.length();
	for (int i = 0; i < len; i++) {
		mipsTokenPrim node;
		if (content[i] == ' ' || content[i] == '\t')continue;
		else if (content[i] >= '0' && content[i] <= '9') {
			node.type = MIPS_TT_DATA;
			node.start = i;

			node.value = float(atoi(content.c_str() + i));

			while (content[i] >= '0' && content[i] <= '9') {
				i++;
			}
			i--;
			node.end = i + 1;
			output.push_back(node);
			continue;
		}
		else if (content[i] >= 'A'&&content[i] <= 'Z' || content[i] >= 'a'&&content[i] <= 'z' || content[i] == '_') {
			node.type = MIPS_TT_SYS;
			node.start = i;
			node.value = 0;
			char str[32];
			int j = 0;
			while (content[i] >= 'A'&&content[i] <= 'Z' || content[i] >= 'a'&&content[i] <= 'z' ||
				content[i] >= '0'&&content[i] <= '9' || content[i] == '_') {
				str[j++] = content[i++];
				if (j == 31) {
					str[j] = '\0';
					error(str, MIPS_LE_TOOLONG);
				}
			}
			str[j] = '\0';
			node.id = hash(str);
			if (node.id > MIPS_ID_JR) {
				node.type = MIPS_TT_USER;
			}

			node.end = i;
			output.push_back(node);
			i--;
			continue;
		}
		else {
			node.type = MIPS_TT_OP;
			node.start = i;
			node.value = 0;
			std::string str;
			char *a = new char[32];
			switch (content[i]) {
			case '$':
				node.type = MIPS_TT_REG;
				if (i + 2 >= (int)content.size())
					error("register", MIPS_LE_INCOMPLETE);
				i++;
				if (content[i] == 'z') {
					if (i + 3 >= (int)content.size())
						error("register", MIPS_LE_INCOMPLETE);
					else if (content[i + 1] == 'e' && content[i + 2] == 'r' && content[i + 3] == 'o')
						node.id = 0;
					else error("register", MIPS_LE_ILLEGAL);
					i += 3;
				}
				else if (content[i] == 'a') {
					if (content[i + 1] == 't')node.id = 1;
					else if (content[i + 1] >= '0' && content[i + 1] <='3')node.id = content[i + 1] - '0' + 4;
					else error("register", MIPS_LE_ILLEGAL);
					i++;
				}
				else if (content[i] == 'v') {
					if (content[i + 1] >= '0' && content[i + 1] <= '1')node.id = content[i + 1] - '0' + 2;
					else error("register", MIPS_LE_ILLEGAL);
					i++;
				}
				else if (content[i] == 't') {
					if (content[i + 1] >= '0' && content[i + 1] <= '7')node.id = content[i + 1] - '0' + 8;
					else if (content[i + 1] >= '8' && content[i + 1] <= '9')node.id = content[i + 1] - '0' + 24;
					else error("register", MIPS_LE_ILLEGAL);
					i++;
				}
				else if (content[i] == 's') {
					if (content[i + 1] >= '0' && content[i + 1] <= '7')node.id = content[i + 1] - '0' + 16;
					else if (content[i + 1] == 'p')node.id = 29;
					else error("register", MIPS_LE_ILLEGAL);
					i++;
				}
				else if (content[i] == 'k') {
					if (content[i + 1] >= '0' && content[i + 1] <= '1')node.id = content[i + 1] - '0' + 26;
					else error("register", MIPS_LE_ILLEGAL);
					i++;
				}
				else if (content[i] == 'g') {
					if (content[i + 1] == 'p')node.id = 28;
					else error("register", MIPS_LE_ILLEGAL);
					i++;
				}
				else if (content[i] == 'f') {
					if (content[i + 1] == 'p')node.id = 30;
					else error("register", MIPS_LE_ILLEGAL);
					i++;
				}
				else if (content[i] == 'r') {
					if (content[i + 1] == 'a')node.id = 31;
					else error("register", MIPS_LE_ILLEGAL);
					i++;
				}
				node.end = i + 1;
				output.push_back(node);
				continue;
			case '+':
				node.id = MIPS_OP_PLUS;
				node.end = i + 1;
				output.push_back(node);
				continue;
			case '-':
				node.id = MIPS_OP_MINUS;
				node.end = i + 1;
				output.push_back(node);
				continue;
			case '*':
				node.id = MIPS_OP_MULTY;
				node.end = i + 1;
				output.push_back(node);
				continue;
			case '/':
				node.id = MIPS_OP_DIVIDE;
				node.end = i + 1;
				output.push_back(node);
				continue;
			case '%':
				node.id = MIPS_OP_MOD;
				node.end = i + 1;
				output.push_back(node);
				continue;
			case '&':
				node.id = MIPS_OP_AND;
				node.end = i + 1;
				output.push_back(node);
				continue;
			case '|':
				node.id = MIPS_OP_OR;
				node.end = i + 1;
				output.push_back(node);
				continue;
			case '^':
				node.id = MIPS_OP_NOR;
				node.end = i + 1;
				output.push_back(node);
				continue;
			case '~':
				node.id = MIPS_OP_INVERSE;
				node.end = i + 1;
				output.push_back(node);
				continue;
			case '(':
				node.id = MIPS_OP_LBRACE;
				node.end = i + 1;
				output.push_back(node);
				continue;
			case ')':
				node.id = MIPS_OP_RBRACE;
				node.end = i + 1;
				output.push_back(node);
				continue;
			case '[':
				node.id = MIPS_OP_LPARENTHESIS;
				node.end = i + 1;
				output.push_back(node);
				continue;
			case ']':
				node.id = MIPS_OP_RPARENTHESIS;
				node.end = i + 1;
				output.push_back(node);
				continue;
			case '{':
				node.id = MIPS_OP_LBRAKET;
				node.end = i + 1;
				output.push_back(node);
				continue;
			case '}':
				node.id = MIPS_OP_RBRAKET;
				node.end = i + 1;
				output.push_back(node);
				continue;
			case ';':
				node.id = MIPS_OP_SEMI;
				node.end = i + 1;
				output.push_back(node);
				continue;
			case ',':
				node.id = MIPS_OP_COMMA;
				node.end = i + 1;
				output.push_back(node);
				continue;
			case '.':
				node.id = MIPS_OP_DOT;
				node.end = i + 1;
				output.push_back(node);
				continue;
			case  '!':
				node.id = MIPS_OP_NOT;
				node.end = i + 1;
				output.push_back(node);
				continue;
			case ':':
				node.id = MIPS_OP_COLON;
				node.end = i + 1;
				output.push_back(node);
				continue;
			case '#':
				while (++i <  (int)content.size() && content[i] != '\n');
				continue;
			}
		}
	}
	return output;
}
std::vector<std::pair<int, std::pair<int, int>>> MipsLex::highlight() {
	std::vector<std::pair<int, std::pair<int, int>>> ret;

	try {
		parse();
	}
	catch (MIPSLexemeException *e) {
		delete e;
	}
	for (auto t : output) {
		ret.push_back(std::pair<MIPSTOKENTYPE, std::pair<int, int>>(t.type, std::pair<int, int>(t.start, t.end)));
	}

	return ret;
}
std::vector<std::string> MipsLex::complete(std::string tab) {
	std::vector<std::string> ret;

	for (auto s : strId) {
		if (s.find(tab) == 0)ret.push_back(s);
	}

	return ret;
}

const char *MipsLex::get() {
	return content.c_str();
}

void MipsLex::error(const char *word, int type) {
	switch (type) {
	case MIPS_LE_ILLEGAL:
		throw new MIPSLexemeException(string("非法的标识符") + word);
	case MIPS_LE_TOOLONG:
		throw new MIPSLexemeException(string("标识符太长：") + word);
	case MIPS_LE_INCOMPLETE:
		throw new MIPSLexemeException(string("结构不完整") + word);
	case MIPS_LE_EXPOP:
		throw new MIPSLexemeException(string("符号错误") + word);
	}
}
