#include "operation.h"
#include "editor.h"

extern Editor *editor;

Operation::Operation() {

}
Operation::~Operation() {

}

void Operation::add(int file, int preBlock, int postBlock,
	int preLine, int postLine,int preOffset, int postOffset, char ch) {
	Step tmp;
	tmp.type = OP_INSERT;
	tmp.file = file;
	tmp.preBlock = preBlock;
	tmp.postBlock = postBlock;
	tmp.preLine = preLine;
	tmp.postLine = postLine;
	tmp.prePos = preOffset;
	tmp.postPos = postOffset;
	tmp.content.push_back(std::string() + ch);
	over.clear();
	past.push_front(tmp);
	if (past.size() > MAX_QUEUE_LENGTH)past.pop_back();
}
void Operation::add(int file, int preBlock, int postBlock,
	int preLine, int postLine, int preOffset, int postOffset, std::vector<std::string> str) {
	Step tmp;
	tmp.type = OP_PASTE;
	tmp.file = file;
	tmp.preBlock = preBlock;
	tmp.postBlock = postBlock;
	tmp.preLine = preLine;
	tmp.postLine = postLine;
	tmp.prePos = preOffset;
	tmp.postPos = postOffset;
	tmp.content = str;
	over.clear();
	past.push_front(tmp);
	if (past.size() > MAX_QUEUE_LENGTH)past.pop_back();
}
void Operation::del(int file, int preBlock, int postBlock,
	int preLine, int postLine, int preOffset, int postOffset, char ch) {
	Step tmp;
	tmp.type = OP_DELETE;
	tmp.file = file;
	tmp.preBlock = preBlock;
	tmp.postBlock = postBlock;
	tmp.preLine = preLine;
	tmp.postLine = postLine;
	tmp.prePos = preOffset;
	tmp.postPos = postOffset;
	tmp.content.push_back(std::string() + ch);
	over.clear();
	past.push_front(tmp);
	if (past.size() > MAX_QUEUE_LENGTH)past.pop_back();
}
void Operation::del(int file, int preBlock, int postBlock,
	int preLine, int postLine, int preOffset, int postOffset, std::vector<std::string> str) {
	Step tmp;
	tmp.type = OP_CUT;
	tmp.file = file;
	tmp.preBlock = preBlock;
	tmp.postBlock = postBlock;
	tmp.preLine = preLine;
	tmp.postLine = postLine;
	tmp.prePos = preOffset;
	tmp.postPos = postOffset;
	tmp.content = str;
	over.clear();
	past.push_front(tmp);
	if (past.size() > MAX_QUEUE_LENGTH)past.pop_back();
}

Step Operation::regret() {
	if (past.empty())return Step();
	Step tmp = past.front();
	over.push_back(tmp);
	past.pop_front();
	return tmp;
}
Step Operation::ahead() {
	if (over.empty())return Step();
	Step tmp = over.back();
	over.pop_back();
	past.push_front(tmp);
	return tmp;
}

void Operation::setCutPre(int preBlock, int preLine, int preOffset) {
	over.back().preBlock = preBlock;
	over.back().preLine = preLine;
	over.back().prePos = preOffset;
}