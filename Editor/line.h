#pragma once
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include "winsgl.h"

using std::string;
using std::vector;

class Line {
private:
	string *content;
	Line *next, *prev;

	int focusPos = 0;

	int selectBegin = -1;
	int selectEnd = -1;

	friend class Block;
	friend class File;
	friend class Editor;
public:
	Line(string str = string());
	Line(string *c) :content(c), next(NULL) {}
	Line(vector<string> &cont, int &count, int offset = 0);
	~Line();

	string insert(char c);
	int remove();
	void append(string str);
	string cut(int pos = -1);
};