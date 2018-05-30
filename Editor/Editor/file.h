#pragma once
#include "block.h"

class File {
private:
	string filename;
	string format;

	Block *content;
	File *next, *prev;

	int start = 0;
	int total = 0;
	int focus = -1;

	int selectBegin = -1;
	int selectEnd = -1;

	friend class Editor;
public:
	int debugLine = 0;

	File();
	File(char *filename);
	~File();

	string getPath();
	string getDir();
	string getName();
	string getFormat();
	void setPath(string name);

	Block *startBlock();
	Block *focusBlock();
	Block *blockAt(int n);
	int totalLine();
	int startLine();
	void setStart(int line);
	void setFocus(int line, int size, int delta);
};
