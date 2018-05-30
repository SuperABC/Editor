#pragma once
#include "file.h"
#include "lang.h"
#include "popup.h"
#include "operation.h"

#define WINDOW_WIDTH 1280
#define WINDOW_HEIGHT 720

#define DEBUGGER_WIDTH 320

//#define DICT_THREE

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
	bool ctrling = false;
	bool editing = false;
	vecTwo clickPos;

	bool panelWork = false;
	bool panelUsed = false;
	vecTwo panelPos;

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
	inline void unselect() {
		File *tmpFile = actFile();
		Block *tmpBlock = tmpFile->blockAt(tmpFile->selectBegin);
		Line *tmpLine = tmpBlock->lineAt(tmpBlock->selectBegin);
		tmpLine->selectBegin = tmpLine->selectEnd = -1;
		tmpBlock->selectBegin = tmpBlock->selectEnd = -1;
		tmpFile->selectBegin = tmpFile->selectEnd = -1;
	}
public:
	int fontSize;
	string fontName;

	vecTwo fileListBase;
	vecTwo fileListSize;
	vecTwo outputWindowBase;
	vecTwo outputWindowSize;
	vecTwo editorBase;
	vecTwo editorSize;
	vecTwo cursorPos;

	RGB cursorColor;
	RGB lnColor;
	RGB selectColor;

	Language *lex = NULL;
	Language *general = NULL;
	bool debugging = false;
	Debugger *dbg = NULL;

	Popup pop;

#ifndef DICT_THREE
	int ratio[128][128];
#else
	float ratio[128][128][128];
#endif

	Operation history;

	Editor(string config = string());
	~Editor();

	void dict();
	void layout();
	void resize(int x, int y);
	void create(char *dir);
	void open(char *filename);
	void save(char *filename = NULL);
	void rename();
	void close();
	void cut(bool hist = true);
	vector<string> copy();
	void paste(vector<string> cont, bool hist = true);
	void choose();
	void tab();
	void untab();
	void pre();
	void post();
	void home();
	void end();
	void lineup();
	void linedown();
	void start();
	void eof();
	void undo();
	void redo();
	void script();
	void git();
	void round();
	void through();

	void key(int key, bool hist = true);
	void mouse(vecThree mouse);
	void drag(vecTwo pos);

	void refresh();

	void logPrint(const char *log);
	void logClear();

	friend void chname(string data);

	File *actFile();
	int getNum();
};

