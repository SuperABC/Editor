#define _CRT_SECURE_NO_WARNINGS
#include "lexeme.h"

Lexeme::Lexeme(const char *input) {
	for (int i = 0; i < 256; i++) {
		list[i] = NULL;
	}
	idNum = 0;
	prepare();

	if (input == NULL)return;
	content = std::string(input);
}
Lexeme::~Lexeme() {

}

int Lexeme::preserve(const char *str) {
	int value = 0;
	int len = strlen(str);
	for (int i = 0; i < len; i++) {
		value += str[i];
	}
	value %= 256;
	hashNode *tmp = list[value];
	while (tmp != NULL && strcmp(tmp->name.c_str(), str) != 0) {
		tmp = tmp->next;
	}
	if (tmp == NULL) {
		tmp = new hashNode();
		tmp->id = idNum++;
		tmp->type = TT_SYS;
		tmp->name = string(str);
		tmp->next = list[value];
		list[value] = tmp;
		strId.push_back(string(str));
	}
	return tmp->id;
}
void Lexeme::prepare() {
	preserve("integer");
	preserve("char");
	preserve("string");
	preserve("float");
	preserve("bool");
	preserve("array");
	preserve("class");
	preserve("function");
	preserve("none");
	preserve("let");
	preserve("be");
	preserve("new");
	preserve("with");
	preserve("start");
	preserve("end");
	preserve("if");
	preserve("then");
	preserve("else");
	preserve("loop");
	preserve("when");
	preserve("of");
	preserve("is");
	preserve("are");
	preserve("smaller");
	preserve("larger");
	preserve("than");
	preserve("use");
	preserve("library");
	preserve("result");
	preserve("quit");
	preserve("and");
	preserve("or");
	preserve("not");
	preserve("xor");
	preserve("break");
	preserve("redo");
	preserve("comment");
}
int Lexeme::hash(string str) {
	int value = 0;
	int len = str.length();
	for (int i = 0; i < len; i++) {
		value += str[i];
	}
	value %= 256;
	hashNode *tmp = list[value];
	while (tmp != NULL && str != tmp->name) {
		tmp = tmp->next;
	}
	if (tmp == NULL) {
		tmp = new hashNode();
		tmp->id = idNum++;
		tmp->type = TT_USER;
		tmp->name = *new std::string(str);
		tmp->next = list[value];
		list[value] = tmp;
		strId.push_back(str);
	}
	return tmp->id;
}

Lexeme *Lexeme::input(const char *str) {
	content = std::string(str);
	return this;
}
vector<tokenPrim> Lexeme::parse() {
	output.clear();

	int len = content.length();
	for (int i = 0; i < len; i++) {
		tokenPrim *node = new tokenPrim();
		if (content[i] == ' '|| content[i] == '\t')continue;
		else if (content[i] >= '0' && content[i] <= '9') {
			node->type = TT_DATA;
			node->start = i;

			int t = i;
			while (content[t] >= '0'&&content[t] <= '9') {
				t++;
			}
			if (content[t] == '.' && ++t != content.size() && content[t] >= '0'&&content[t] <= '9') {
				node->value = float(atof(content.c_str() + i));
				node->id = CT_FLOAT;
			}
			else {
				node->value = float(atoi(content.c_str() + i));
				node->id = CT_INT;
			}

			i++;
			bool point = false;
			while (content[i] >= '0' && content[i] <= '9' || content[i] == '.') {
				if (content[i] == '.') {
					if (point)break;
					else point = true;
				}
				i++;
			}
			i--;
			if (content[i] == '.')i--;

			node->end = i + 1;
			output.push_back(*node);
			continue;
		}
		else if (content[i] >= 'A'&&content[i] <= 'Z' || content[i] >= 'a'&&content[i] <= 'z' || content[i] == '_') {
			node->type = TT_SYS;
			node->start = i;
			node->value = 0;
			char *str = (char *)malloc(32);
			int j = 0;
			while (content[i] >= 'A'&&content[i] <= 'Z' || content[i] >= 'a'&&content[i] <= 'z' ||
				content[i] >= '0'&&content[i] <= '9' || content[i] == '_') {
				str[j++] = content[i++];
				if (j == 31) {
					str[j] = '\0';
					error(str, LE_TOOLONG);
				}
			}
			str[j] = '\0';
			node->id = hash(str);
			if (node->id > ID_COMMENT) {
				node->type = TT_USER;
			}

			node->end = i;
			output.push_back(*node);
			i--;
			continue;
		}
		else {
			node->type = TT_OP;
			node->start = i;
			node->value = 0;
			std::string str;
			char *a = new char[32];
			switch (content[i]) {
			case '+':
				if (content[i + 1] == '+') {
					node->id = OP_PLUSPLUS;
					i++;
					node->end = i + 1;
					output.push_back(*node);
					continue;
				}
				else if (content[i + 1] == '=') {
					node->id = OP_EQPLUS;
					i++;
					node->end = i + 1;
					output.push_back(*node);
					continue;
				}
				else {
					node->id = OP_PLUS;
					node->end = i + 1;
					output.push_back(*node);
					continue;
				}
			case '-':
				if (content[i + 1] == '-') {
					node->id = OP_MINUSMINUS;
					i++;
					node->end = i + 1;
					output.push_back(*node);
					continue;
				}
				else if (content[i + 1] == '=') {
					node->id = OP_EQMINUS;
					i++;
					node->end = i + 1;
					output.push_back(*node);
					continue;
				}
				else {
					node->id = OP_MINUS;
					node->end = i + 1;
					output.push_back(*node);
					continue;
				}
			case '*':
				if (content[i + 1] == '=') {
					node->id = OP_EQMULTY;
					i++;
					node->end = i + 1;
					output.push_back(*node);
					continue;
				}
				else {
					node->id = OP_MULTY;
					node->end = i + 1;
					output.push_back(*node);
					continue;
				}
			case '/':
				if (content[i + 1] == '=') {
					node->id = OP_EQDIVIDE;
					i++;
					node->end = i + 1;
					output.push_back(*node);
					continue;
				}
				else {
					node->id = OP_DIVIDE;
					node->end = i + 1;
					output.push_back(*node);
					continue;
				}
			case '%':
				if (content[i + 1] == '=') {
					node->id = OP_EQMOD;
					i++;
					node->end = i + 1;
					output.push_back(*node);
					continue;
				}
				else {
					node->id = OP_MOD;
					node->end = i + 1;
					output.push_back(*node);
					continue;
				}
			case '&':
				if (content[i + 1] == '&') {
					node->id = OP_ANDAND;
					i++;
					node->end = i + 1;
					output.push_back(*node);
					continue;
				}
				else if (content[i + 1] == '=') {
					node->id = OP_EQAND;
					i++;
					node->end = i + 1;
					output.push_back(*node);
					continue;
				}
				else {
					node->id = OP_AND;
					node->end = i + 1;
					output.push_back(*node);
					continue;
				}
			case '|':
				if (content[i + 1] == '|') {
					node->id = OP_OROR;
					i++;
					node->end = i + 1;
					output.push_back(*node);
					continue;
				}
				else if (content[i + 1] == '=') {
					node->id = OP_EQOR;
					i++;
					node->end = i + 1;
					output.push_back(*node);
					continue;
				}
				else {
					node->id = OP_OR;
					node->end = i + 1;
					output.push_back(*node);
					continue;
				}
			case '^':
				if (content[i + 1] == '=') {
					node->id = OP_EQNOR;
					i++;
					node->end = i + 1;
					output.push_back(*node);
					continue;
				}
				else {
					node->id = OP_NOR;
					node->end = i + 1;
					output.push_back(*node);
					continue;
				}
			case '~':
				if (content[i + 1] == '=') {
					node->id = OP_EQINVERSE;
					i++;
					node->end = i + 1;
					output.push_back(*node);
					continue;
				}
				else {
					node->id = OP_INVERSE;
					node->end = i + 1;
					output.push_back(*node);
					continue;
				}
			case '(':
				node->id = OP_LBRACE;
				node->end = i + 1;
				output.push_back(*node);
				continue;
			case ')':
				node->id = OP_RBRACE;
				node->end = i + 1;
				output.push_back(*node);
				continue;
			case '[':
				node->id = OP_LPARENTHESIS;
				node->end = i + 1;
				output.push_back(*node);
				continue;
			case ']':
				node->id = OP_RPARENTHESIS;
				node->end = i + 1;
				output.push_back(*node);
				continue;
			case '{':
				node->id = OP_LBRAKET;
				node->end = i + 1;
				output.push_back(*node);
				continue;
			case '}':
				node->id = OP_RBRAKET;
				node->end = i + 1;
				output.push_back(*node);
				continue;
			case '>':
				if (content[i + 1] == '=') {
					node->id = OP_NSMALLER;
					i++;
					node->end = i + 1;
					output.push_back(*node);
					continue;
				}
				else {
					node->id = OP_GREATER;
					node->end = i + 1;
					output.push_back(*node);
					continue;
				}
			case '<':
				if (content[i + 1] == '=') {
					node->id = OP_NGREATER;
					i++;
					node->end = i + 1;
					output.push_back(*node);
					continue;
				}
				else {
					node->id = OP_SMALLER;
					node->end = i + 1;
					output.push_back(*node);
					continue;
				}
			case ';':
				node->id = OP_SEMI;
				node->end = i + 1;
				output.push_back(*node);
				continue;
			case ',':
				node->id = OP_COMMA;
				node->end = i + 1;
				output.push_back(*node);
				continue;
			case '.':
				node->id = OP_DOT;
				node->end = i + 1;
				output.push_back(*node);
				continue;
			case  '!':
				if (content[i + 1] == '=') {
					node->id = OP_NOTEQ;
					i++;
					node->end = i + 1;
					output.push_back(*node);
					continue;
				}
				else {
					node->id = OP_NOT;
					node->end = i + 1;
					output.push_back(*node);
					continue;
				}
			case '=':
				node->id = OP_EQUAL;
				node->end = i + 1;
				output.push_back(*node);
				continue;
			case '?':
				node->id = OP_QUERY;
				node->end = i + 1;
				output.push_back(*node);
				continue;
			case '\"':
				node->type = TT_DATA;
				i++;
				while (content[i] != '\"'&& i < (int)content.length()) {
					str.push_back(content[i++]);
				}
				if (content[i] != '\"')
					error("", LE_INCOMPLETE);
				strcpy(a, str.c_str());
				node->id = CT_STRING;
				node->s = a;
				node->end = i + 1;
				output.push_back(*node);
				continue;
			case '\'':
				if (content[++i] == 's') {
					node->type = TT_SYS;
					node->id = ID_OF;
					node->end = i + 1;
					output.push_back(*node);
				}
				else error("\'s", LE_EXPOP);
				continue;
			case '#':
				while (++i <  (int)content.size() && content[i] != '\n');
				continue;
			}
		}
	}
	return output;
}
std::vector<std::pair<TOKENTYPE, std::pair<int, int>>> Lexeme::highlight() {
	std::vector<std::pair<TOKENTYPE, std::pair<int, int>>> ret;

	try {
		parse();
	}
	catch (LexemeException *e) {}
	for (auto t : output) {
		ret.push_back(std::pair<TOKENTYPE, std::pair<int, int>>(t.type, std::pair<int, int>(t.start, t.end)));
	}

	return ret;
}

const char *Lexeme::get() {
	return content.c_str();
}

void Lexeme::error(const char *word, int type) {
	switch (type) {
	case LE_ILLEGAL:
		throw new LexemeException(string("非法的标识符") + word);
	case LE_TOOLONG:
		throw new LexemeException(string("标识符太长：") + word);
	case LE_INCOMPLETE:
		throw new LexemeException(string("结构不完整") + word);
	case LE_EXPOP:
		throw new LexemeException(string("符号错误") + word);
	}
}
