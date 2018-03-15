#include "editor.h"

void putStringExt(Cstring str, int x, int y) {
	int tab = 0;
	for (unsigned int i = 0; i < strlen(str); i++) {
		if (str[i] == '\t')tab++;
	}
	char *tmp = (char *)malloc(strlen(str) + 3 * tab + 1);
	memset(tmp, 0, strlen(str) + 3 * tab + 1);
	for (unsigned int i = 0, j = 0; i < strlen(str); i++) {
		if (str[i] == '\t') {
			tmp[j] = tmp[j + 1] = tmp[j + 2] = tmp[j + 3] = ' ';
			j += 4;
		}
		else tmp[j++] = str[i];
	}
	putString(tmp, x, y);
}
int stringWidthExt(Cstring str, int offset) {
	int tab = 0;
	if (offset < 0)return 0;

	for (int i = 0; i < offset; i++) {
		if (str[i] == '\t')tab++;
	}
	char *tmp = (char *)malloc(offset + 3 * tab + 1);
	memset(tmp, 0, offset + 3 * tab + 1);
	for (int i = 0, j = 0; i < offset; i++) {
		if (str[i] == '\t') {
			tmp[j] = tmp[j + 1] = tmp[j + 2] = tmp[j + 3] = ' ';
			j += 4;
		}
		else tmp[j++] = str[i];
	}
	return stringWidth(tmp, strlen(tmp));
}

Editor::Editor() {
	content = NULL;

	editorSize.x = 560;
	editorSize.y = 480;
	cursorPos.x = cursorPos.y = 0;
	fontSize = 28;
	fontName = "Comic Sans MS";
}
Editor::~Editor() {

}

void Editor::create() {
	if (content == NULL) {
		content = new File();
	}
	else {
		File *tmp = content;
		while (tmp->next) {
			tmp = tmp->next;
		}
		tmp->next = new File();
	}

	active = fileNum++;
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
		tmp->next->prev = tmp;
	}

	active = fileNum++;
}
void Editor::save() {
	File *actFile = this->actFile();

	if (active == -1)return;
	else {
		std::ofstream fout(actFile->filename, std::ios::out);

		Block *tmpBlock = actFile->content;
		while (tmpBlock) {
			Line *tmpLine = tmpBlock->content;
			while (tmpLine) {
				fout << tmpLine->content->c_str() << std::endl;
				tmpLine = tmpLine->next;
			}
			tmpBlock = tmpBlock->next;
		}

		fout.close();
	}
}
void Editor::close() {
	File *tmp = content, *link;

	if (active == -1)return;
	else if (active == 0) {
		if (fileNum == 1) {
			active = -1;
			cursorPos.x = cursorPos.y = 0;
		}
		link = content->next;
		delete content;
		content = link;
	}
	else {
		for (int i = 1; i < active; i++)tmp = tmp->next;
		active--;
		link = tmp->next->next;
		delete tmp->next;
		tmp->next = link;
	}

	fileNum--;
	refresh();
}
void Editor::refresh() {
	setColor(64, 64, 64);
	clearScreen();
	if (active == -1)return;

	File *actFile = this->actFile();

	int x = 0, y = 0;
	Block *tmpBlock = actFile->startBlock();

	setColor(255, 255, 255);
	setFontSize(fontSize);
	setFontName((char *)fontName.c_str());

	while (tmpBlock) {
		Line *tmpLine = tmpBlock->startLine();
		while (tmpLine) {
			putStringExt((Cstring)tmpLine->content->c_str(), x, y);
			y += fontSize + 4;
			if (y > EDITOR_HEIGHT)break;
			tmpLine = tmpLine->next;
		}
		if (y > EDITOR_HEIGHT)break;
		tmpBlock = tmpBlock->next;
	}

	tmpBlock = actFile->focusBlock();
	Line *tmpLine = tmpBlock->focusLine();
	cursorPos.x = stringWidthExt((char *)tmpLine->content->c_str(), tmpLine->focusPos);
}

void Editor::key(int key) {
	if (active == -1)return;

	File *actFile = this->actFile();
	if (key == SG_DOWN) {
		Block *tmpBlock = actFile->focusBlock();
		Line *tmpLine = tmpBlock->focusLine();
		if (tmpBlock->focus < tmpBlock->total - 1) {
			tmpBlock->focus++;
			tmpLine->next->focusPos = tmpLine->focusPos;
			if (tmpLine->next->focusPos > (int)tmpLine->next->content->length())
				tmpLine->next->focusPos = tmpLine->next->content->length();
		}
		else {
			if (tmpBlock->next) {
				tmpBlock->focus = -1;
				actFile->focus++;
				tmpBlock = tmpBlock->next;
				tmpBlock->focus = 0;
				tmpBlock->content->focusPos = tmpLine->focusPos;
				if (tmpBlock->content->focusPos > (int)tmpBlock->content->content->length())
					tmpBlock->content->focusPos = tmpBlock->content->content->length();
			}
			else return;
		}

		if (cursorPos.y + 2 * (fontSize + 4) > editorSize.y) {
			tmpBlock = actFile->startBlock();
			if (tmpBlock->start < tmpBlock->total - 1) {
				tmpBlock->start++;
			}
			else {
				if (tmpBlock->next) {
					tmpBlock->start = 0;
					actFile->start++;
					tmpBlock = tmpBlock->next;
					tmpBlock->start = 0;
				}
			}
		}
		else {
			cursorPos.y += fontSize + 4;
		}
	}
	if (key == SG_UP) {
		Block *tmpBlock = actFile->focusBlock();
		Line *tmpLine = tmpBlock->focusLine();
		if (tmpBlock->focus > 0) {
			int focusPos = tmpLine->focusPos;

			tmpBlock->focus--;
			tmpLine = tmpBlock->focusLine();
			tmpLine->focusPos = focusPos;
			if (tmpLine->focusPos > (int)tmpLine->content->length())
				tmpLine->focusPos = tmpLine->content->length();
		}
		else {
			if (actFile->focus > 0) {
				int focusPos = tmpLine->focusPos;

				tmpBlock->focus = -1;
				actFile->focus--;
				tmpBlock = tmpBlock->prev;
				tmpBlock->focus = tmpBlock->total - 1;
				tmpLine = tmpBlock->focusLine();
				tmpLine->focusPos = focusPos;
				if (tmpLine->focusPos > (int)tmpLine->content->length())
					tmpLine->focusPos = tmpLine->content->length();
			}
		}

		if (cursorPos.y == 0) {
			tmpBlock = actFile->startBlock();
			if (tmpBlock->start > 0)tmpBlock->start--;
			else {
				if (actFile->start > 0) {
					tmpBlock->start = 0;
					actFile->start--;
					tmpBlock = tmpBlock->prev;
					tmpBlock->start = tmpBlock->total - 1;
				}
			}
		}
		else {
			cursorPos.y -= fontSize + 4;
		}
	}
	if (key == SG_LEFT) {
		Block *tmpBlock = actFile->focusBlock();
		Line *tmpLine = tmpBlock->focusLine();
		if (tmpLine->focusPos == 0) {
			return;
		}
		else {
			tmpLine->focusPos--;
		}
	}
	if (key == SG_RIGHT) {
		Block *tmpBlock = actFile->focusBlock();
		Line *tmpLine = tmpBlock->focusLine();
		if (tmpLine->focusPos > (int)tmpLine->content->size() - 1) {
			return;
		}
		else {
			tmpLine->focusPos++;
		}
	}
	if (key >= 0x20 && key < 0x80) {
		Block *tmpBlock = actFile->focusBlock();
		tmpBlock->focusLine()->insert(key);
		tmpBlock->size++;
	}
	if (key == '\b') {
		Block *tmpBlock = actFile->focusBlock();
		Line *tmpLine = tmpBlock->focusLine();
		if (tmpLine->remove()) {
			string left = tmpLine->content->c_str();
			if (tmpLine->prev) {
				if (tmpLine->next) {
					tmpLine->prev->next = tmpLine->next;
					tmpLine->next = NULL;
					tmpLine = tmpLine->prev;
					delete tmpLine->next->prev;
					tmpLine->next->prev = tmpLine;
				}
				else {
					tmpLine = tmpLine->prev;
					delete tmpLine->next;
					tmpLine->next = NULL;
				}
				tmpLine->focusPos = tmpLine->content->length();
				tmpLine->append(left);

				tmpBlock->focus--;
				tmpBlock->total--;
			}
			else if (tmpBlock->prev) {
				if (tmpLine->next) {
					tmpBlock->content = tmpLine->next;
					tmpLine->next = NULL;
					delete tmpBlock->content->prev;
					tmpBlock->content->prev = NULL;
					tmpBlock->focus = -1;
					tmpBlock->total--;
					tmpBlock = tmpBlock->prev;
					actFile->focus--;
					tmpBlock->focus = tmpBlock->total - 1;
					tmpLine = tmpBlock->focusLine();
				}
				else {
					tmpBlock = tmpBlock->prev;
					tmpBlock->focus = tmpBlock->total - 1;
					tmpLine = tmpBlock->focusLine();
					if (tmpBlock->next->next) {
						tmpBlock->next = tmpBlock->next->next;
						tmpBlock->next->prev->next = NULL;
						delete tmpBlock->next->prev;
						tmpBlock->next->prev = tmpBlock;
					}
					else {
						delete tmpBlock->next;
						tmpBlock->next = NULL;
					}

					actFile->focus--;
					actFile->total--;
				}
				tmpLine->focusPos = tmpLine->content->length();
				tmpLine->append(left);
			}
			else return;

			if (cursorPos.y == 0) {
				tmpBlock = actFile->startBlock();
				if (tmpBlock->start > 0)tmpBlock->start--;
				else {
					if (actFile->start > 0) {
						tmpBlock->start = 0;
						actFile->start--;
						tmpBlock = tmpBlock->prev;
						tmpBlock->start = tmpBlock->total - 1;
					}
				}
			}
			else {
				cursorPos.y -= fontSize + 4;
			}
		}
		else {
			tmpBlock->size--;
		}
	}
	if (key == '\r') {
		Block *tmpBlock = actFile->focusBlock();
		Line *tmpLine = tmpBlock->focusLine();
		string more = tmpLine->cut();

		if (tmpBlock->size > BLOCK_SIZE) {
			if (tmpBlock->next) {
				tmpBlock->next->prev = new Block(more);
				tmpBlock->next->prev->next = tmpBlock->next;
				tmpBlock->next = tmpBlock->next->prev;
				tmpBlock->next->prev = tmpBlock;
				actFile->total++;
				actFile->focus++;
			}
			else {
				tmpBlock->next = new Block(more);
				tmpBlock->next->prev = tmpBlock;
				actFile->total++;
				actFile->focus++;
			}
		}
		else {
			if (tmpLine->next) {
				tmpLine->next->prev = new Line(more);
				tmpLine->next->prev->next = tmpLine->next;
				tmpLine->next = tmpLine->next->prev;
				tmpLine->next->prev = tmpLine;
				tmpBlock->total++;
				tmpBlock->focus++;
			}
			else {
				tmpLine->next = new Line(more);
				tmpLine->next->prev = tmpLine;
				tmpBlock->total++;
				tmpBlock->focus++;
			}
		}
		
		if (cursorPos.y + 2 * (fontSize + 4) > editorSize.y) {
			tmpBlock = actFile->startBlock();
			if (tmpBlock->start < tmpBlock->total - 1) {
				tmpBlock->start++;
			}
			else {
				if (tmpBlock->next) {
					tmpBlock->start = 0;
					actFile->start++;
					tmpBlock = tmpBlock->next;
					tmpBlock->start = 0;
				}
			}
		}
		else {
			cursorPos.y += fontSize + 4;
		}
	}
	refresh();
}
void Editor::mouse(vecThree mouse) {

}

File *Editor::actFile() {
	File *actFile = content;
	for (int i = 0; i < active; i++)actFile = actFile->next;
	return actFile;
}
