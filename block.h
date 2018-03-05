#pragma once
#include "line.h"

#define BLOCK_SIZE 4096

class Block {
private:
	Line *content;
	Block *next;

	int startLine = -1;
	int totalLine = 0;

	friend class File;
	friend class Editor;
public:
	Block(vector<string> &cont, int &count, int offset = 0);
	~Block();
};
