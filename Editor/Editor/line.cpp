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

string Line::insert(char c) {
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

	if (c == ' ')return "";

	int wordOff = focusPos - 1;
	char tmpChar = content->operator[](wordOff);
	bool text = false;
	if (tmpChar >= 'a' && tmpChar <= 'z' || tmpChar >= 'A' && tmpChar <= 'Z' ||
		tmpChar == '_' || tmpChar >= '0' && tmpChar <= '9')
		text = true;
	wordOff--;
	if (text) {
		while (wordOff >= 0) {
			tmpChar = content->operator[](wordOff);
			if (tmpChar >= 'a' && tmpChar <= 'z' || tmpChar >= 'A' && tmpChar <= 'Z' ||
				tmpChar == '_' || tmpChar >= '0' && tmpChar <= '9') {
				wordOff--;
				continue;
			}
			else break;
		}
		if (wordOff < 0 || !(tmpChar >= 'a' && tmpChar <= 'z' || tmpChar >= 'A' && tmpChar <= 'Z' ||
			tmpChar == '_' || tmpChar >= '0' && tmpChar <= '9'))wordOff++;
	}
	else {
		while (wordOff >= 0) {
			tmpChar = content->operator[](wordOff);
			if (!(tmpChar >= 'a' && tmpChar <= 'z' || tmpChar >= 'A' && tmpChar <= 'Z' ||
				tmpChar == '_' || tmpChar >= '0' && tmpChar <= '9') && tmpChar != ' ' && tmpChar != '\t') {
				wordOff--;
				continue;
			}
			else break;
		}
		if (wordOff < 0 || ((tmpChar >= 'a' && tmpChar <= 'z' || tmpChar >= 'A' && tmpChar <= 'Z' ||
			tmpChar == '_' || tmpChar >= '0' && tmpChar <= '9') || tmpChar == ' ' || tmpChar == '\t'))wordOff++;
	}

	/*int wordOff = content->rfind(' ', focusPos - 1) + 1;
	while (content->operator[](wordOff) == '\t')wordOff++;*/
	return string(content->begin() + wordOff, content->begin() + focusPos);
}
void Line::complete(string str) {
	int wordOff = focusPos - 1;
	char tmpChar = content->operator[](wordOff);
	bool text = false;
	if (tmpChar >= 'a' && tmpChar <= 'z' || tmpChar >= 'A' && tmpChar <= 'Z' ||
		tmpChar == '_' || tmpChar >= '0' && tmpChar <= '9')
		text = true;
	wordOff--;
	if (text) {
		while (wordOff >= 0) {
			tmpChar = content->operator[](wordOff);
			if (tmpChar >= 'a' && tmpChar <= 'z' || tmpChar >= 'A' && tmpChar <= 'Z' ||
				tmpChar == '_' || tmpChar >= '0' && tmpChar <= '9') {
				wordOff--;
				continue;
			}
			else break;
		}
		if (wordOff < 0 || !(tmpChar >= 'a' && tmpChar <= 'z' || tmpChar >= 'A' && tmpChar <= 'Z' ||
			tmpChar == '_' || tmpChar >= '0' && tmpChar <= '9'))wordOff++;
	}
	else {
		while (wordOff >= 0) {
			tmpChar = content->operator[](wordOff);
			if (!(tmpChar >= 'a' && tmpChar <= 'z' || tmpChar >= 'A' && tmpChar <= 'Z' ||
				tmpChar == '_' || tmpChar >= '0' && tmpChar <= '9') && tmpChar != ' ' && tmpChar != '\t') {
				wordOff--;
				continue;
			}
			else break;
		}
		if (wordOff < 0 || ((tmpChar >= 'a' && tmpChar <= 'z' || tmpChar >= 'A' && tmpChar <= 'Z' ||
			tmpChar == '_' || tmpChar >= '0' && tmpChar <= '9') || tmpChar == ' ' || tmpChar == '\t'))wordOff++;
	}

	*content = string(content->begin(), content->begin() + wordOff) + str +
		string(content->begin() + focusPos, content->end());
	focusPos = wordOff + str.length();
}
int Line::remove() {
	if (focusPos == 0)return 1;

	if (content->size() == focusPos) {
		content->pop_back();
		focusPos--;
		selectBegin = selectEnd = focusPos;
	}
	else {
		for (unsigned int i = focusPos; i < content->size(); i++) {
			(*content)[i - 1] = (*content)[i];
		}
		content->pop_back();
		focusPos--;
		selectBegin = selectEnd = focusPos;
	}
	return 0;
}
void Line::append(string str) {
	*content += str;
}
string Line::cut(int pos) {
	string ret;
	if (pos == -1) {
		ret = content->data() + focusPos;
		*content = content->substr(0, focusPos);
	}
	else {
		ret = content->data() + pos;
		*content = content->substr(0, pos);
	}
	return ret;
}
