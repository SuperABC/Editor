#pragma once
#include "file.h"

#define EDITOR_WIDTH 640
#define EDITOR_HEIGHT 480

class Editor {
private:
	File *content;
	int fileNum = 0;

	int active = -1;

public:
	Editor();
	~Editor();

	void open(char *filename);
	void refresh();
	void close();
	void save();

	void key(int key);
	void mouse(vecThree mouse);
};

