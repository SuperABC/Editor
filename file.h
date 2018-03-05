#pragma once
#include "block.h"

class File {
private:
	char *filename;

	Block *content;
	File *next;

	int startBlock = -1;
	int totalBlock = 0;

	friend class Editor;
public:
	File(char *filename);
	~File();
};
