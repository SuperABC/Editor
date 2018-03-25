#pragma once
#include "line.h"

#define BLOCK_SIZE 4096

class Block {
private:
	Line *content;
	Block *next, *prev;

	int start = 0;
	int total = 0;
	int focus = -1;

	int selectBegin = -1;
	int selectEnd = -1;

	int size = 0;

	friend class File;
	friend class Editor;
public:
	Block(string str = string());
	Block(vector<string> &cont, int &count, int offset = 0);
	~Block();

	Line *startLine();
	Line *focusLine();
	Line *lineAt(int n);
};
