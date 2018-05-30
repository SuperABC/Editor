#ifndef CPP_LEXEME_H
#define CPP_LEXEME_H
#include <cstdio>
#include <iostream>
#include <vector>
#include <string>
#include "../Editor/lang.h"

using std::vector;
using std::string;

enum CPPTOKENTYPE {
	CPP_TT_USER,
	CPP_TT_DATA,
	CPP_TT_OP,
	CPP_TT_SYS
};

enum CPPIDENTIFIER {
	CPP_ID_ASM,
	CPP_ID_AUTO,
	CPP_ID_BOOL,
	CPP_ID_BREAK,
	CPP_ID_CASE,
	CPP_ID_CATCH,
	CPP_ID_CHAR,
	CPP_ID_CLASS,
	CPP_ID_CONST,
	CPP_ID_CONST_CAST,
	CPP_ID_CONITNUE,
	CPP_ID_DEFAULT,
	CPP_ID_DELETE,
	CPP_ID_DO,
	CPP_ID_DOUBLE,
	CPP_ID_DYNAMIC_CAST,
	CPP_ID_ELSE,
	CPP_ID_ENUM,
	CPP_ID_EXPLICIT,
	CPP_ID_EXPORT,
	CPP_ID_EXTERN,
	CPP_ID_FALSE,
	CPP_ID_FLOAT,
	CPP_ID_FOR,
	CPP_ID_FRIEND,
	CPP_ID_GOTO,
	CPP_ID_IF,
	CPP_ID_INLINE,
	CPP_ID_INT,
	CPP_ID_LONG,
	CPP_ID_MUTABLE,
	CPP_ID_NAMESPACE,
	CPP_ID_NEW,
	CPP_ID_OPERATOR,
	CPP_ID_PRIVATE,
	CPP_ID_PROTECTED,
	CPP_ID_PUBLIC,
	CPP_ID_REGISTER,
	CPP_ID_REINTERPRET_CAST,
	CPP_ID_RETURN,
	CPP_ID_SHORT,
	CPP_ID_SIGNED,
	CPP_ID_SIZEOF,
	CPP_ID_STATIC,
	CPP_ID_STATIC_CAST,
	CPP_ID_STRUCT,
	CPP_ID_SWITCH,
	CPP_ID_TEMPLATE,
	CPP_ID_THIS,
	CPP_ID_THROW,
	CPP_ID_TRUE,
	CPP_ID_TRY,
	CPP_ID_TYPEDEF,
	CPP_ID_TYPEID,
	CPP_ID_TYPENAME,
	CPP_ID_UNION,
	CPP_ID_UNSIGNED,
	CPP_ID_USING,
	CPP_ID_VIRTUAL,
	CPP_ID_VOID,
	CPP_ID_VOLATILE,
	CPP_ID_WCHAR_T,
	CPP_ID_WHILE
};
enum CPPOPERATOR {
	CPP_OP_PLUS,
	CPP_OP_PLUSPLUS,
	CPP_OP_EQPLUS,
	CPP_OP_MINUS,
	CPP_OP_MINUSMINUS,
	CPP_OP_EQMINUS,
	CPP_OP_NEG,
	CPP_OP_MULTY,
	CPP_OP_EQMULTY,
	CPP_OP_DIVIDE,
	CPP_OP_EQDIVIDE,
	CPP_OP_MOD,
	CPP_OP_EQMOD,
	CPP_OP_AND,
	CPP_OP_ANDAND,
	CPP_OP_EQAND,
	CPP_OP_OR,
	CPP_OP_OROR,
	CPP_OP_EQOR,
	CPP_OP_NOR,
	CPP_OP_EQNOR,
	CPP_OP_INVERSE,
	CPP_OP_EQINVERSE,
	CPP_OP_LBRACE,
	CPP_OP_RBRACE,
	CPP_OP_LPARENTHESIS,
	CPP_OP_RPARENTHESIS,
	CPP_OP_LBRAKET,
	CPP_OP_RBRAKET,
	CPP_OP_SEMI,
	CPP_OP_COMMA,
	CPP_OP_DOT,
	CPP_OP_SMALLER,
	CPP_OP_NSMALLER,
	CPP_OP_GREATER,
	CPP_OP_NGREATER,
	CPP_OP_NOT,
	CPP_OP_NOTEQ,
	CPP_OP_EQUAL,
	CPP_OP_QUERY,
	CPP_OP_QUOT,
	CPP_OP_DBQUOT,
	CPP_OP_COLON,
	CPP_OP_DOMAIN,
	CPP_OP_PRE,
};

typedef struct _cppHn {
	enum CPPTOKENTYPE type;
	std::string name;
	int id;
	struct _cppHn *next;
}cppHashNode;
typedef struct {
	enum CPPTOKENTYPE type;
	int id;
	float value;
	char *s = NULL;

	int line;
	int start, end;
}cppTokenPrim;

class CppLex : public Language {
private:
	string content;
	int idNum;

	cppHashNode *list[256];

	int tmpLine = 0;
public:
	vector<cppTokenPrim> output;
	vector<string> strId;

	CppLex(const char *input = NULL);
	virtual ~CppLex();

	int preserve(const char *str);
	void prepare();
	int hash(string s);

	CppLex *input(const char *str);
	std::vector<cppTokenPrim>parse();
	std::vector<std::pair<int, std::pair<int, int>>> highlight();
	std::vector<std::string> complete(std::string tab);

	const char *get();

	static void error(const char *word, int type);
};

enum CPPLEXEMEERROR {
	CPP_LE_ILLEGAL,
	CPP_LE_TOOLONG,
	CPP_LE_INCOMPLETE,
	CPP_LE_EXPOP
};
class  CPPLexemeException {
private:
	std::string msg;
public:
	CPPLexemeException(std::string s) {
		msg = s;
	}
	const char *message() { return msg.data(); }
};
#endif