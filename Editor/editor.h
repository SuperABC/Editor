#pragma once
#include "file.h"

#define WINDOW_WIDTH 1280
#define WINDOW_HEIGHT 720

void logClearCall(widgetObj *w, int x, int y, int status);
void logCopyCall(widgetObj *w, int x, int y, int status);

class Editor {
private:
	File *content;
	int fileNum = 0;

	int active = -1;

public:
	vecTwo editorBase;
	vecTwo editorSize;
	vecTwo fileListBase;
	vecTwo fileListSize;
	vecTwo outputWindowBase;
	vecTwo outputWindowSize;
	vecTwo cursorPos;

	int fontSize;
	string fontName;
	RGB cursorColor;

	Editor(string config = string());
	~Editor();

	void layout();
	void resize(int x, int y);
	void create(char *dir);
	void open(char *filename);
	void refresh();
	void close();
	void save(char *filename = NULL);

	void key(int key);
	void mouse(vecThree mouse);

	void logPrint(const char *log);
	void logClear();

	File *actFile();
};

