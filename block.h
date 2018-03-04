#pragma once
#include "line.h"

class Block {
private:
	Line *content;
	Block *next;

	friend class File;
public:
	Block() :content(NULL), next(NULL) {};
};
