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
	Line *next;

	friend class Block;
	friend class File;
	friend class Editor;
public:
	Line(string *c) :content(c), next(NULL) {}
	Line(vector<string> &cont, int &count, int offset = 0);
	~Line();
	
	char get();

	void append(char c);
	void change(int pos, char c);
	void remove(int pos);
};