#pragma once
#include "block.h"

class File {
private:
	string filename;

	Block *content;
	File *next, *prev;

	int start = 0;
	int total = 0;
	int focus = -1;

	friend class Editor;
public:
	File();
	File(char *filename);
	~File();

	Block *startBlock();
	Block *focusBlock();
};
