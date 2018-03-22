#pragma once
#include <vector>

class Operation {
private:
	enum OPERATION {
		OP_INSERT,
		OP_NEWLINE,
		OP_DELETE,
		OP_COPY,
		OP_CUT,
		OP_PASTE
	};

	std::vector<std::pair<std::pair<int, int>, std::pair<int, std::string>>> list;
	int tmpPos;
public:
	Operation();
	~Operation();

	void add(int line, int offset, char ch);
	void add(int line, int offset, std::string str);
	void del(int line, int offset, char ch);
	void del(int line, int offset, std::string str);
};