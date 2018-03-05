#include "editor.h"

Editor::Editor() {
	content = NULL;
}
Editor::~Editor() {

}

void Editor::open(char *filename) {
	if (content == NULL) {
		content = new File(filename);
	}
	else {
		File *tmp = content;
		while (tmp->next) {
			tmp = tmp->next;
		}
		tmp->next = new File(filename);
	}

	active = fileNum++;
}
void Editor::save() {
	File *actFile = content;

	if (active == -1)return;
	else {
		for (int i = 0; i < active; i++)actFile = actFile->next;
		std::ofstream fout(actFile->filename);

		Block *tmpBlock = actFile->content;
		while (tmpBlock) {
			Line *tmpLine = tmpBlock->content;
			while (tmpLine) {
				fout << tmpLine->content << std::endl;
				tmpLine = tmpLine->next;
			}
			tmpBlock = tmpBlock->next;
		}

		fout.close();
	}
}
void Editor::close() {
	File *tmp = content;

	if (active == -1)return;
	else if (active == 0) {
		active = -1;
		delete content;
		content = NULL;
	}
	else {
		for (int i = 1; i < active; i++)tmp = tmp->next;
		active--;
		delete tmp->next;
		tmp->next = NULL;
	}

	fileNum--;
	refresh();
}
void Editor::refresh() {
	File *actFile = content;

	setColor(64, 64, 64);
	clearScreen();
	if (active == -1)return;
	for (int i = 0; i < active; i++)actFile = actFile->next;

	setColor(255, 255, 255);

	int x = 0, y = 0;
	Block *tmpBlock = actFile->content;
	for (int i = 0; i < actFile->startBlock; i++)tmpBlock = tmpBlock->next;
	while (tmpBlock) {
		Line *tmpLine = tmpBlock->content;
		for (int i = 0; i < tmpBlock->startLine; i++)tmpLine = tmpLine->next;
		while (tmpLine) {
			putString((Cstring)tmpLine->content->c_str(), x, y);
			y += 24;
			if (y > EDITOR_HEIGHT)return;
			tmpLine = tmpLine->next;
		}
		tmpBlock = tmpBlock->next;
	}
}

void Editor::key(int key) {
	if (active == -1)return;
	if (key == SG_DOWN) {
		File *actFile = content;
		for (int i = 0; i < active; i++)actFile = actFile->next;

		Block *tmpBlock = actFile->content;
		for (int i = 0; i < actFile->startBlock; i++)tmpBlock = tmpBlock->next;
		if (tmpBlock->startLine < tmpBlock->totalLine - 1) {
			tmpBlock->startLine++;
		}
		else {
			if (tmpBlock->next) {
				tmpBlock->startLine = 0;
				actFile->startBlock++;
				tmpBlock = tmpBlock->next;
				tmpBlock->startLine = 0;
			}
		}

		refresh();
	}
	if (key == SG_UP) {
		File *actFile = content;
		for (int i = 0; i < active; i++)actFile = actFile->next;

		Block *tmpBlock = actFile->content;
		for (int i = 0; i < actFile->startBlock; i++)tmpBlock = tmpBlock->next;
		if(tmpBlock->startLine > 0)tmpBlock->startLine--;
		else {
			if (actFile->startBlock > 0) {
				tmpBlock->startLine = 0;
				actFile->startBlock--;
				tmpBlock = actFile->content;
				for (int i = 0; i < actFile->startBlock; i++)tmpBlock = tmpBlock->next;
				tmpBlock->startLine = tmpBlock->totalLine - 1;
			}
		}

		refresh();
	}
}
void Editor::mouse(vecThree mouse) {

}
