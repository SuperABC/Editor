#pragma once
#include "Editor/file.h"

class Editor {
private:
	File * content;

	int fileNum = 0;
	int active = -1;

	void scroll(int line) {
		File *actFile = this->actFile();
		if (line > 0) {
			Block *tmpBlock = actFile->startBlock();
			if (tmpBlock->start + line < tmpBlock->total) {
				tmpBlock->start += line;
			}
			else if (tmpBlock->next) {
				line -= tmpBlock->total - tmpBlock->start;
				while (tmpBlock->next) {
					tmpBlock->start = 0;
					actFile->start++;
					tmpBlock = tmpBlock->next;
					if (line < tmpBlock->total)break;
					line -= tmpBlock->total;
				}
				if (line < tmpBlock->total)
					tmpBlock->start = line;
				else tmpBlock->start = tmpBlock->total - 1;
			}
			else {
				tmpBlock->start = tmpBlock->total - 1;
			}
		}
		else if (line < 0) {
			line = -line;
			Block *tmpBlock = actFile->startBlock();
			if (tmpBlock->start >= line)tmpBlock->start -= line;
			else if (tmpBlock->prev) {
				line -= tmpBlock->start;
				while (tmpBlock->prev) {
					tmpBlock->start = 0;
					actFile->start--;
					tmpBlock = tmpBlock->prev;
					if (line < tmpBlock->total)break;
					line -= tmpBlock->total;
				}
				if (line < tmpBlock->total)
					tmpBlock->start = tmpBlock->total - line;
				else tmpBlock->start = 0;
			}
			else {
				tmpBlock->start = 0;
			}
		}
	}
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
	vecTwo cursorPos;

	Editor(string config = string());
	~Editor();

	void create(char *dir);
	void open(char *filename);
	void save(char *filename = NULL);
	void close();
	void insert(char c);
	void operate(int c);
	void cut();
	vector<string> copy();
	void paste(vector<string> cont);
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
	void round();
	void through();

	File *actFile();
	int getNum();
};

