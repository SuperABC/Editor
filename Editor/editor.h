#pragma once
#include "file.h"

#define WINDOW_WIDTH 1280
#define WINDOW_HEIGHT 720

void logClearCall(widgetObj *w, int x, int y, int status);
void logCopyCall(widgetObj *w, int x, int y, int status);
void scrollVertMove(widgetObj *w, int x, int y);
void scrollVertClick(widgetObj *w, int x, int y, int status);
void scrollHorizMove(widgetObj *w, int x, int y);
void scrollHorizClick(widgetObj *w, int x, int y, int status);

class Editor {
private:
	File *content;
	int fileNum = 0;

	int active = -1;
	bool editing = false;

	vecTwo fileListBase;
	vecTwo fileListSize;
	vecTwo outputWindowBase;
	vecTwo outputWindowSize;

	RGB lnColor;
	RGB selectColor;

	void scroll(int line);
	inline bool selecting() {
		File *tmpFile = actFile();
		if (tmpFile->selectBegin != tmpFile->selectEnd)return true;
		Block *tmpBlock = tmpFile->blockAt(tmpFile->selectBegin);
		if (tmpBlock->selectBegin != tmpBlock->selectEnd)return true;
		Line *tmpLine = tmpBlock->lineAt(tmpBlock->selectBegin);
		if (tmpLine->selectBegin != tmpLine->selectEnd)return true;
		return false;
	}
public:
	int fontSize;
	string fontName;
	vecTwo editorBase;
	vecTwo editorSize;
	vecTwo cursorPos;
	RGB cursorColor;

	Editor(string config = string());
	~Editor();

	void layout();
	void resize(int x, int y);
	void create(char *dir);
	void open(char *filename);
	void save(char *filename = NULL);
	void close();
	void cut();
	vector<string> copy();
	void paste(vector<string> cont);
	void choose();
	void tab();
	void untab();
	void refresh();

	void key(int key);
	void mouse(vecThree mouse);
	void drag(vecTwo pos);

	void logPrint(const char *log);
	void logClear();

	File *actFile();
	int getNum();
};

