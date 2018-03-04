#pragma once
#include "block.h"

class File {
private:
	char *filename;

	Block *content;
	File *next;

	friend class Editor;
public:
	File(char *filename) : filename(filename), content(NULL), next(NULL) {};
};
