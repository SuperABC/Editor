#pragma once
#include <vector>
#include <list>

#define MAX_QUEUE_LENGTH 1024

enum OPERATION {
	OP_NULL,
	OP_INSERT,
	OP_DELETE,
	OP_CUT,
	OP_PASTE
};
struct Step {
	OPERATION type;
	int file, preBlock, postBlock, preLine, postLine, prePos, postPos;
	std::vector<std::string> content;
	Step() : type(OP_NULL) {}
};

class Operation {
private:
	int capacity = 256;
	int position = -1;
	std::list<Step> past, over;
public:

	Operation();
	~Operation();

	void add(int file, int preBlock, int postBlock,
		int preLine, int postLine, int preOffset, int postOffset, char ch);
	void add(int file, int preBlock, int postBlock,
		int preLine, int postLine, int preOffset, int postOffset, std::vector<std::string> str);
	void del(int file, int preBlock, int postBlock,
		int preLine, int postLine, int preOffset, int postOffset, char ch);
	void del(int file, int preBlock, int postBlock,
		int preLine, int postLine, int preOffset, int postOffset, std::vector<std::string> str);

	Step regret();
	Step ahead();

	void setCutPre(int preBlock, int preLine, int preOffset);
};