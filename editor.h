#pragma once
#include "file.h"

class Editor {
private:
	File *content;

public:
	Editor();
	~Editor();

	void open(char *filename);
	void close();
	void save();

	void key(int key);
	void mouse(vecThree mouse);
};

