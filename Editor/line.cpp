#include "line.h"

Line::Line(string str) : next(NULL), prev(NULL) {
	content = new string(str);
	focusPos = 0;
}
Line::Line(vector<string> &cont, int &count, int offset) :
	content(NULL), next(NULL), prev(NULL) {
	count++;

	content = new string(cont[offset]);
	if (offset + 1 < (int)cont.size()) {
		next = new Line(cont, count, offset + 1);
		next->prev = this;
	}
}
Line::~Line() {
	if (next)delete next;
	delete content;
}

void Line::insert(char c) {
	if (content->size() == focusPos) {
		*content += c;
		focusPos++;
	}
	else {
		*content += (*content)[content->size() - 1];
		for (int i = content->size() - 2; i > focusPos; i--) {
			(*content)[i] = (*content)[i - 1];
		}
		(*content)[focusPos++] = c;
	}
}
int  Line::remove() {
	if (focusPos == 0)return 1;

	if (content->size() == focusPos) {
		content->pop_back();
		focusPos--;
	}
	else {
		for (unsigned int i = focusPos; i < content->size(); i++) {
			(*content)[i - 1] = (*content)[i];
		}
		content->pop_back();
		focusPos--;
	}
	return 0;
}
void Line::append(string str) {
	*content += str;
}
string Line::cut() {
	string ret = content->c_str() + focusPos;
	*content = content->substr(0, focusPos);
	return ret;
}
