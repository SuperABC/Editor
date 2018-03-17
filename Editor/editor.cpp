#include "editor.h"
#include "../SGS/lexeme.h"
#include "../Util/json.h"

Editor::Editor(string config) {
	content = NULL;

	editorBase.x = 160;
	editorBase.y = 20;
	editorSize.x = 1120;
	editorSize.y = 600;
	fileListBase.x = 0;
	fileListBase.y = 0;
	fileListSize.x = 120;
	fileListSize.y = 720;
	outputWindowBase.x = 120;
	outputWindowBase.y = 600;
	outputWindowSize.x = 1160;
	outputWindowSize.y = 120;

	std::ifstream fin(config);
	if (!fin.is_open()) {
		cursorPos.x = cursorPos.y = 0;
		fontSize = 28;
		fontName = "Comic Sans MS";
		cursorColor.r = cursorColor.g = cursorColor.b = 255;

		create((char *)"");
	}
	else {
		string stream, tmp;
		while (!fin.eof()) {
			getline(fin, tmp);
			stream += tmp;
		}

		JSON json((char *)stream.c_str());
		fontSize = (int)json["fontSize"].f;
		fontName = json["fontName"].str;
		cursorColor.r = (int)json["cursorColor"].array[0];
		cursorColor.g = (int)json["cursorColor"].array[1];
		cursorColor.b = (int)json["cursorColor"].array[2];
		int fileNum = (int)json["fileNum"].f;
		if (fileNum == 0) {
			create((char *)"");
		}
		else {
			char fileName[16];
			for (int i = 0; i < fileNum; i++) {
				sprintf_s(fileName, 16, "file%d", i);
				open(json[fileName].str);
			}
			active = (int)json["active"].f;
		}
	}

	layout();
}
Editor::~Editor() {

}

void Editor::layout() {
	widgetObj *logPanle;
	logPanle = newWidget(SG_OUTPUT, (SGstring)"log");
	logPanle->pos.x = outputWindowBase.x;
	logPanle->pos.y = outputWindowBase.y;
	logPanle->size.x = outputWindowSize.x;
	logPanle->size.y = outputWindowSize.y;
	logPanle->bgColor.r = logPanle->bgColor.g = logPanle->bgColor.b = 31;
	logPanle->fgColor.r = logPanle->fgColor.g = logPanle->fgColor.b = 255;
	registerWidget(logPanle);
	free(logPanle);

	widgetObj *logClear, *logCopy;
	logClear = newWidget(SG_BUTTON, (SGstring)"clear");
	logClear->pos.x = outputWindowBase.x + outputWindowSize.x - 108;
	logClear->pos.y = outputWindowBase.y + 8;
	logClear->size.x = 100;
	logClear->size.y = 32;
	strcpy_s((char *)logClear->content, INFINITE, "清除输出");
	logClear->mouseClick = (mouseClickCall)logClearCall;
	registerWidget(logClear);
	free(logClear);
	logCopy = newWidget(SG_BUTTON, (SGstring)"copy");
	logCopy->pos.x = outputWindowBase.x + outputWindowSize.x - 108;;
	logCopy->pos.y = outputWindowBase.y + 48;
	logCopy->size.x = 100;
	logCopy->size.y = 32;
	strcpy_s((char *)logCopy->content, INFINITE, "复制文本");
	logCopy->mouseClick = (mouseClickCall)logCopyCall;
	registerWidget(logCopy);
	free(logCopy);
}
void Editor::resize(int x, int y) {
	if (x == 0 || y == 0)return;
	editorBase.x = 160;
	editorBase.y = 20;
	editorSize.x = x - 160;
	editorSize.y = y - 120 - 20;;
	fileListBase.x = 0;
	fileListBase.y = 0;
	fileListSize.x = 120;
	fileListSize.y = y;
	outputWindowBase.x = 120;
	outputWindowBase.y = y - 120;
	outputWindowSize.x = x - 120;
	outputWindowSize.y = 120;

	widgetObj *log = getWidgetByName("log");
	log->pos = outputWindowBase;
	log->size = outputWindowSize;
	widgetObj *logClear = getWidgetByName("clear");
	logClear->pos.x = outputWindowBase.x + outputWindowSize.x - 108;
	logClear->pos.y = outputWindowBase.y + 8;
	widgetObj *logCopy = getWidgetByName("copy");
	logCopy->pos.x = outputWindowBase.x + outputWindowSize.x - 108;
	logCopy->pos.y = outputWindowBase.y + 48;
}
void Editor::create(char *dir) {
	if (content == NULL) {
		content = new File();
		content->filename = string(dir) + '\\' + content->filename;
	}
	else {
		File *tmp = content;
		while (tmp->next) {
			tmp = tmp->next;
		}
		tmp->next = new File();
		tmp->next->filename = string(dir) + '\\' + tmp->next->filename;
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
void Editor::save(char *filename) {
	File *actFile = this->actFile();

	if (active == -1)return;
	else {
		if (filename != NULL) {
			actFile->filename = filename;
			if (actFile->filename.rfind('.') == -1)actFile->format = string();
			else actFile->format = string(actFile->filename.begin() + actFile->filename.rfind('.') + 1, actFile->filename.end());
		}
		std::ofstream fout(actFile->filename, std::ios::out);

		Block *tmpBlock = actFile->content;
		while (tmpBlock) {
			Line *tmpLine = tmpBlock->content;
			while (tmpLine) {
				fout << tmpLine->content->c_str();
				if(tmpLine->next || tmpBlock->next)fout << std::endl;
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

	//Main frame.
	setColor(64, 64, 64);
	clearScreen();
	setColor(127, 127, 127);
	putQuad(editorBase.x - 40, editorBase.y - 20, editorBase.x + editorSize.x - 1, editorBase.y - 1, SOLID_FILL);
	setColor(95, 95, 95);
	putQuad(fileListBase.x + 1, fileListBase.y + 1,
		fileListBase.x + fileListSize.x - 2, fileListBase.y + fileListSize.y - 2, EMPTY_FILL);
	setColor(255, 255, 255);
	setFontSize(20);
	setFontName((char *)"微软雅黑");
	putString((char *)"文件列表", 0, 0);
	setColor(31, 31, 31);
	putQuad(editorBase.x - 40, editorBase.y,
		editorBase.x + editorSize.x - 1, editorBase.y + editorSize.y - 1, SOLID_FILL);
	if (active == -1)return;

	//Edit area.
	File *actFile = this->actFile();

	int x = editorBase.x, y = editorBase.y;
	Block *tmpBlock = actFile->startBlock();

	setFontSize(fontSize);
	setFontName((char *)fontName.c_str());

	while (tmpBlock) {
		Line *tmpLine = tmpBlock->startLine();
		while (tmpLine) {
			if (actFile->format == "sgs") {
				Lexeme lex = Lexeme();

				lex.input(tmpLine->content->c_str());
				std::vector<std::pair<TOKENTYPE, std::pair<int, int>>> hl = lex.highlight();
				string tmp;
				for (unsigned int i = 0; i < hl.size(); i++) {
					switch (hl[i].first) {
					case TT_USER:
						setColor(255, 255, 255);
						break;
					case TT_SYS:
						setColor(223, 159, 127);
						break;
					case TT_OP:
						setColor(191, 191, 63);
						break;
					case TT_DATA:
						setColor(95, 159, 223);
						break;
					default:
						setColor(255, 255, 255);
						break;
					}
					tmp = string(tmpLine->content->begin() + hl[i].second.first, tmpLine->content->begin() + hl[i].second.second);

					int px = x + stringWidth((Cstring)tmpLine->content->c_str(), hl[i].second.first);
					int py = y;
					putString((Cstring)tmp.c_str(), px, py);
				}
				if (!hl.empty() && hl[hl.size() - 1].second.second < (int)tmpLine->content->size()) {
					tmp = string(tmpLine->content->begin() + hl[hl.size() - 1].second.second, tmpLine->content->end());
					int px = x + stringWidth((Cstring)tmpLine->content->c_str(), hl[hl.size() - 1].second.second);
					int py = y;
					setColor(95, 159, 63);
					putString((Cstring)tmp.c_str(), px, py);
				}
				else if (hl.empty() && tmpLine->content->size() > 0) {
					setColor(95, 159, 63);
					putString((Cstring)tmpLine->content->c_str(), x, y);
				}
			}
			else {
				setColor(255, 255, 255);
				putString((Cstring)tmpLine->content->c_str(), x, y);
			}
			y += fontSize + 4;
			if (y > editorSize.y)break;
			tmpLine = tmpLine->next;
		}
		if (y > editorSize.y)break;
		tmpBlock = tmpBlock->next;
	}

	//Cursor.
	tmpBlock = actFile->focusBlock();
	Line *tmpLine = tmpBlock->focusLine();
	cursorPos.x = stringWidth((char *)tmpLine->content->c_str(), tmpLine->focusPos);
	if (actFile->focus == actFile->start) {
		cursorPos.y = (actFile->startBlock()->focus - actFile->startBlock()->start) * (fontSize + 4);
	}
	else {
		int delta = 0;
		delta += tmpBlock->total - tmpBlock->start;
		tmpBlock = tmpBlock->next;
		for (int i = 0; i < actFile->focus - actFile->start - 1; i++) {
			delta += tmpBlock->total;
			tmpBlock = tmpBlock->next;
		}
		delta += tmpBlock->focus;
		cursorPos.y = delta * (fontSize + 4);
	}

	//File list.
	File *tmpFile = this->content;
	int listPos = 0, fileIdx = 0;
	setColor(255, 255, 255);
	setFontSize(20);
	setFontName((char *)"微软雅黑");
	while (tmpFile) {
		if (fileIdx == active) {
			setColor(31, 31, 31);
			putQuad(0, 20 + listPos, 120, 20 + listPos + 20, SOLID_FILL);
			setColor(255, 255, 255);
		}
		putString((char *)tmpFile->getName().c_str(), 20, 20 + listPos);
		listPos += 20;
		fileIdx++;
		tmpFile = tmpFile->next;
	}
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
		clearKeyBuffer();
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
		clearKeyBuffer();
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
		clearKeyBuffer();
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
		clearKeyBuffer();
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
		}
		else {
			tmpBlock->size--;
		}
		clearKeyBuffer();
	}
	if (key == '\r') {
		Block *tmpBlock = actFile->focusBlock();
		Line *tmpLine = tmpBlock->focusLine();
		string more = tmpLine->cut();

		int tab = 0;
		while (tmpLine->content->operator[](tab++) == '\t') {
			more = '\t' + more;
		}
		if (tmpBlock->size > BLOCK_SIZE) {
			if (tmpBlock->next) {
				tmpBlock->next->prev = new Block(more);
				tmpBlock->next->prev->content->focusPos = tab - 1;
				tmpBlock->next->prev->next = tmpBlock->next;
				tmpBlock->next = tmpBlock->next->prev;
				tmpBlock->next->prev = tmpBlock;
				actFile->total++;
				actFile->focus++;
			}
			else {
				tmpBlock->next = new Block(more);
				tmpBlock->next->content->focusPos = tab - 1;
				tmpBlock->next->prev = tmpBlock;
				actFile->total++;
				actFile->focus++;
			}
		}
		else {
			if (tmpLine->next) {
				tmpLine->next->prev = new Line(more);
				tmpLine->next->prev->focusPos = tab - 1;
				tmpLine->next->prev->next = tmpLine->next;
				tmpLine->next = tmpLine->next->prev;
				tmpLine->next->prev = tmpLine;
				tmpBlock->total++;
				tmpBlock->focus++;
			}
			else {
				tmpLine->next = new Line(more);
				tmpLine->next->focusPos = tab - 1;
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
		clearKeyBuffer();
	}
	if (key == '\t') {
		Block *tmpBlock = actFile->focusBlock();
		tmpBlock->focusLine()->insert('\t');
		tmpBlock->size++;
	}
	
	refresh();
}
void Editor::mouse(vecThree mouse) {
	if (active == -1)return;

	File *actFile = this->actFile();

	if (INRECT(mouse.x, mouse.y, editorBase.x, editorBase.y,
		editorBase.x + editorSize.x, editorBase.y + editorSize.y)) {
		if (mouse.m == (SG_BUTTON_DOWN | SG_LEFT_BUTTON)) {
			int emx = mouse.x - editorBase.x, emy = mouse.y - editorBase.y;
			if (emx < 0)emx = 0;
			if (emy < 0)emy = 0;

			int line = emy / (fontSize + 4);
			Block *tmpBlock = actFile->focusBlock();
			if (tmpBlock->total - tmpBlock->start > line) {
				tmpBlock->focus = tmpBlock->start + line;

				setFontSize(fontSize);
				setFontName((char *)fontName.c_str());
				Line *tmpLine = tmpBlock->focusLine();
				int left = 0, right = 0;
				unsigned int i;
				for (i = 0; i < tmpLine->content->length(); i++) {
					if ((right = stringWidth((char *)tmpLine->content->c_str(), i)) <= emx)left = right;
					else break;
				}
				if (2 * emx < left + right)tmpLine->focusPos = i - 1;
				else tmpLine->focusPos = i;
			}
			else {
				line -= tmpBlock->total - tmpBlock->start;
				if (tmpBlock->next) {
					tmpBlock = tmpBlock->next;
					while (line > tmpBlock->total) {
						if (tmpBlock->next) {
							line -= tmpBlock->total;
							tmpBlock = tmpBlock->next;
						}
						else break;
					}
					if (line > tmpBlock->total)
						tmpBlock->focus = tmpBlock->total - 1;
					else
						tmpBlock->focus = line;
				}
				else {
					tmpBlock->focus = tmpBlock->total - 1;
				}

				setFontSize(fontSize);
				setFontName((char *)fontName.c_str());
				Line *tmpLine = tmpBlock->focusLine();
				int left = 0, right = 0;
				unsigned int i;
				for (i = 0; i < tmpLine->content->length(); i++) {
					if ((right = stringWidth((char *)tmpLine->content->c_str(), i)) <= emx)left = right;
					else break;
				}
				if (2 * emx < left + right)tmpLine->focusPos = i - 1;
				else tmpLine->focusPos = i;
			}
		}
	}
	else if (INRECT(mouse.x, mouse.y, 0, 0, 120, WINDOW_HEIGHT)) {
		int line = (mouse.y - 20) / 20;
		if(line < fileNum)active = line;
	}

	refresh();
}

void Editor::logPrint(const char *log) {
	widgetObj *panel = getWidgetByName("log");
	strcat_s((char *)panel->content, INFINITE, log);
}
void Editor::logClear() {
	widgetObj *panel = getWidgetByName("log");
	strcpy_s((char *)panel->content, INFINITE, "");
}

File *Editor::actFile() {
	File *actFile = content;
	for (int i = 0; i < active; i++)actFile = actFile->next;
	return actFile;
}
