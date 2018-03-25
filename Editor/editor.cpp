#define _CRT_SECURE_NO_WARNINGS
#include "editor.h"
#include "popup.h"
#include "../SGS/sgslex.h"
#include "../Mips/mipslex.h"
#include "../Util/json.h"

Editor::Editor(string config) {
	content = NULL;

	editorBase.x = 200;
	editorBase.y = 20;
	editorSize.x = 1080;
	editorSize.y = 580;
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
		lnColor.r = lnColor.g = lnColor.b = 255;
		selectColor.r = selectColor.g = selectColor.b = 255;

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
		lnColor.r = (int)json["lnColor"].array[0];
		lnColor.g = (int)json["lnColor"].array[1];
		lnColor.b = (int)json["lnColor"].array[2];
		selectColor.r = (int)json["selectColor"].array[0];
		selectColor.g = (int)json["selectColor"].array[1];
		selectColor.b = (int)json["selectColor"].array[2];
		int fileNum = (int)json["fileNum"].f;
		if (fileNum == 0) {
			create((char *)"");
		}
		else {
			char fileName[16];
			for (int i = 0; i < fileNum; i++) {
				sprintf(fileName, "file%d", i);
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

	widgetObj *vertScroll;
	vertScroll = newWidget(SG_SCROLLVERT, (SGstring)"vert");
	vertScroll->pos.y = editorBase.y;
	vertScroll->size.y = editorSize.y;
	vertScroll->mouseIn = vertScroll->mouseOut = (mouseMoveCall)scrollVertMove;
	vertScroll->mouseDown = (mouseClickCall)scrollVertClick;
	registerWidget(vertScroll);
	free(vertScroll);
	widgetObj *horizScroll;
	horizScroll = newWidget(SG_SCROLLHORIZ, (SGstring)"horiz");
	horizScroll->pos.x = editorBase.x - 80;
	horizScroll->pos.y = editorBase.y + editorSize.y - 20;
	horizScroll->size.x = editorSize.x + 60;
	horizScroll->mouseIn = horizScroll->mouseOut = (mouseMoveCall)scrollHorizMove;
	horizScroll->mouseDown = (mouseClickCall)scrollHorizClick;
	registerWidget(horizScroll);
	free(horizScroll);
}
void Editor::resize(int x, int y) {
	if (x == 0 || y == 0)return;
	editorBase.x = 200;
	editorBase.y = 20;
	editorSize.x = x - 200;
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

	widgetObj *vertScroll = getWidgetByName("vert");
	vertScroll->pos.x = getWidth(SG_WINDOW) - vertScroll->size.x;
	vertScroll->pos.y = editorBase.y;
	vertScroll->size.y = editorSize.y;
	widgetObj *horizScroll = getWidgetByName("horiz");
	horizScroll->pos.y = getHeight(SG_WINDOW) - horizScroll->size.y - outputWindowSize.y;
	horizScroll->pos.x = editorBase.x - 80;
	horizScroll->size.x = editorSize.x + 60;

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
void Editor::cut() {
	File *actFile = this->actFile();
	if (actFile->selectBegin == actFile->selectEnd) {
		Block *tmpBlock = actFile->blockAt(actFile->selectBegin);
		if (tmpBlock->selectBegin == tmpBlock->selectEnd) {
			Line *tmpLine = tmpBlock->lineAt(tmpBlock->selectBegin);
			string left = tmpLine->cut(tmpLine->selectEnd);
			tmpLine->cut(tmpLine->selectBegin);
			tmpLine->append(left);
			tmpBlock->size -= tmpLine->selectEnd - tmpLine->selectBegin;
			tmpLine->focusPos = tmpLine->selectEnd = tmpLine->selectBegin;
		}
		else {
			tmpBlock->focus = tmpBlock->selectBegin;
			tmpBlock->total -= tmpBlock->selectEnd - tmpBlock->selectBegin;
			if (tmpBlock->focus < tmpBlock->start)tmpBlock->start = tmpBlock->focus;
			Line *beginLine = tmpBlock->lineAt(tmpBlock->selectBegin);
			beginLine->focusPos = beginLine->selectEnd = beginLine->selectBegin;
			Line *endLine = tmpBlock->lineAt(tmpBlock->selectEnd);
			tmpBlock->size -= beginLine->content->length() - beginLine->selectBegin;
			Line *iterLine = beginLine->next;
			while (iterLine != endLine) {
				tmpBlock->size -= iterLine->content->length();
				iterLine = iterLine->next;
			}
			tmpBlock->size -= endLine->selectEnd;
			string left = endLine->cut(endLine->selectEnd);
			beginLine->cut(beginLine->selectBegin);
			beginLine->append(left);
			Line *trashLine = beginLine->next;
			beginLine->next = endLine->next;
			endLine->next = NULL;
			delete trashLine;
			if (beginLine->next)beginLine->next->prev = beginLine;
			tmpBlock->selectEnd = tmpBlock->selectBegin;
		}
	}
	else {
		if (actFile->blockAt(actFile->selectEnd)->total - 1 > actFile->blockAt(actFile->selectEnd)->selectEnd) {
			actFile->total -= actFile->selectEnd - actFile->selectBegin - 1;
			actFile->focus = actFile->selectBegin;
			Block *beginBlock = actFile->blockAt(actFile->selectBegin);
			beginBlock->focus = beginBlock->selectEnd = beginBlock->selectBegin;
			Line *beginLine = beginBlock->focusLine();
			beginLine->focusPos = beginLine->selectEnd = beginLine->selectBegin;
			Block *endBlock = actFile->blockAt(actFile->selectEnd);
			endBlock->focus = endBlock->selectBegin = endBlock->selectEnd;
			Line *endLine = endBlock->focusLine();
			endLine->focusPos = endLine->selectBegin = endLine->selectEnd;
			if (actFile->selectBegin < actFile->start) {
				actFile->start = actFile->selectBegin;
				actFile->startBlock()->start = actFile->startBlock()->selectBegin;
			}
			else if (actFile->selectBegin == actFile->start) {
				if (beginBlock->selectBegin < beginBlock->start)
					beginBlock->start = beginBlock->selectBegin;
			}
			Line *iterLine;
			beginBlock->size -= beginLine->content->length() - beginLine->selectBegin;
			beginBlock->size += endLine->content->length() - endLine->selectEnd;
			iterLine = beginLine->next;
			while (iterLine) {
				beginBlock->size -= iterLine->content->length();
				iterLine = iterLine->next;
			}
			endBlock->size -= endLine->content->length();
			iterLine = endBlock->content;
			while (iterLine != endLine) {
				endBlock->size -= iterLine->content->length();
				iterLine = iterLine->next;
			}
			string left = endLine->cut(endLine->selectEnd);
			beginLine->cut(beginLine->selectBegin);
			beginLine->append(left);
			Line *trashLine;
			trashLine = beginLine->next;
			beginLine->next = NULL;
			delete trashLine;
			beginBlock->total = beginBlock->focus + 1;
			trashLine = endBlock->content;
			endBlock->content = endLine->next;
			endLine->next = NULL;
			delete trashLine;
			endBlock->total -= endBlock->focus + 1;
			endBlock->focus = endBlock->selectBegin = endBlock->selectEnd = -1;
			if (beginBlock->next != endBlock) {
				Block *trashBlock = beginBlock->next;
				endBlock->prev->next = NULL;
				delete trashBlock;
				beginBlock->next = endBlock;
				endBlock->prev = beginBlock;
			}
			actFile->selectEnd = actFile->selectBegin;
		}
		else {
			actFile->total -= actFile->selectEnd - actFile->selectBegin;
			actFile->focus = actFile->selectBegin;
			Block *beginBlock = actFile->blockAt(actFile->selectBegin);
			beginBlock->focus = beginBlock->selectEnd = beginBlock->selectBegin;
			Line *beginLine = beginBlock->focusLine();
			beginLine->focusPos = beginLine->selectEnd = beginLine->selectBegin;
			Block *endBlock = actFile->blockAt(actFile->selectEnd);
			endBlock->focus = endBlock->selectBegin = endBlock->selectEnd;
			Line *endLine = endBlock->focusLine();
			endLine->focusPos = endLine->selectBegin = endLine->selectEnd;
			if (actFile->selectBegin < actFile->start) {
				actFile->start = actFile->selectBegin;
				actFile->startBlock()->start = actFile->startBlock()->selectBegin;
			}
			else if (actFile->selectBegin == actFile->start) {
				if (beginBlock->selectBegin < beginBlock->start)
					beginBlock->start = beginBlock->selectBegin;
			}
			Line *iterLine;
			beginBlock->size -= beginLine->content->length() - beginLine->selectBegin;
			beginBlock->size += endLine->content->length() - endLine->selectEnd;
			iterLine = beginLine->next;
			while (iterLine) {
				beginBlock->size -= iterLine->content->length();
				iterLine = iterLine->next;
			}
			string left = endLine->cut(endLine->selectEnd);
			beginLine->cut(beginLine->selectBegin);
			beginLine->append(left);
			Line *trashLine;
			trashLine = beginLine->next;
			beginLine->next = NULL;
			delete trashLine;
			beginBlock->total = beginBlock->focus + 1;
			Block *trashBlock = beginBlock->next;
			beginBlock->next = endBlock->next;
			endBlock->next = NULL;
			delete trashBlock;
			if(beginBlock->next)beginBlock->next->prev = beginBlock;
			actFile->selectEnd = actFile->selectBegin;
		}
	}
}
vector<string> Editor::copy() {
	vector<string> ret;
	File *actFile = this->actFile();

	if (actFile->selectBegin == actFile->selectEnd) {
		Block *tmpBlock = actFile->blockAt(actFile->selectBegin);
		if (tmpBlock->selectBegin == tmpBlock->selectEnd) {
			Line *tmpLine = tmpBlock->lineAt(tmpBlock->selectBegin);
			ret.push_back(tmpLine->content->substr(tmpLine->selectBegin, tmpLine->selectEnd - tmpLine->selectBegin));
		}
		else {
			Line *beginLine = tmpBlock->lineAt(tmpBlock->selectBegin);
			Line *endLine = tmpBlock->lineAt(tmpBlock->selectEnd);
			ret.push_back(beginLine->content->substr(beginLine->selectBegin,
				beginLine->content->length() - beginLine->selectBegin) + '\n');
			beginLine = beginLine->next;
			while (beginLine != endLine) {
				ret.push_back(*beginLine->content + '\n');
				beginLine = beginLine->next;
			}
			ret.push_back(beginLine->content->substr(0, beginLine->selectEnd));
		}
	}
	else {
		Block *beginBlock = actFile->blockAt(actFile->selectBegin);
		Block *endBlock = actFile->blockAt(actFile->selectEnd);
		Line *beginLine = beginBlock->lineAt(beginBlock->selectBegin);
		Line *endLine = endBlock->lineAt(endBlock->selectEnd);
		ret.push_back(beginLine->content->substr(beginLine->selectBegin,
			beginLine->content->length() - beginLine->selectBegin) + '\n');
		beginLine = beginLine->next;
		while (beginLine) {
			ret.push_back(*beginLine->content + '\n');
			beginLine = beginLine->next;
		}
		beginBlock = beginBlock->next;
		while (beginBlock != endBlock) {
			beginLine = beginBlock->content;
			while (beginLine) {
				ret.push_back(*beginLine->content + '\n');
				beginLine = beginLine->next;
			}
			beginBlock = beginBlock->next;
		}
		beginLine = beginBlock->content;
		while (beginLine != endLine) {
			ret.push_back(*beginLine->content + '\n');
			beginLine = beginLine->next;
		}
		ret.push_back(beginLine->content->substr(0, beginLine->selectEnd));
	}

	return ret;
}
void Editor::paste(vector<string> cont) {
	if (selecting())cut();
	File *actFile = this->actFile();
	Block *tmpBlock = actFile->focusBlock();
	Line *tmpLine = tmpBlock->focusLine();
	int tmpFocus = tmpLine->content->length() - tmpLine->focusPos;
	cont[cont.size() - 1].append(tmpLine->cut());
	tmpLine->append(cont[0]);
	for (int i = 1; i < cont.size(); i++) {
		if (tmpBlock->size > BLOCK_SIZE) { // miss size change
			if (tmpBlock->next) {
				tmpBlock->next->prev = new Block(cont[i]);
				tmpBlock->next->prev->content->next = tmpLine->next;
				tmpBlock->next->prev->content->next->prev = tmpBlock->next->prev->content;
				tmpLine->next = NULL;
				tmpBlock->total = tmpBlock->focus + 1;
				tmpBlock->focus =tmpBlock->selectBegin = tmpBlock->selectEnd = -1;
				tmpBlock->next->prev->next = tmpBlock->next;
				tmpBlock->next = tmpBlock->next->prev;
				tmpBlock->next->prev = tmpBlock;
				tmpBlock->next->focus = tmpBlock->next->selectBegin = tmpBlock->next->selectEnd = 0;
				tmpBlock->next->content->focusPos =
					tmpBlock->next->content->selectBegin = tmpBlock->next->content->selectEnd =
					tmpBlock->next->content->content->length();
				Line *iterLine = tmpBlock->content;
				tmpBlock->size = 0;
				while (iterLine) {
					tmpBlock->size += iterLine->content->length();
					iterLine = iterLine->next;
				}
				tmpBlock = tmpBlock->next;
				tmpBlock->size = cont[i].length();
				tmpLine = tmpBlock->content;
				actFile->focus = actFile->selectBegin = actFile->selectEnd = actFile->focus + 1;
				actFile->total++;
			}
			else {
				tmpBlock->next = new Block(cont[i]);
				tmpBlock->next->prev = tmpBlock;
				tmpBlock->next->next = NULL;
				tmpBlock->next->content->next = tmpLine->next;
				if(tmpLine->next)
					tmpLine->next->prev = tmpBlock->next->content;
				tmpLine->next = NULL;
				tmpBlock->total = tmpBlock->focus + 1;
				tmpBlock->focus = tmpBlock->selectBegin = tmpBlock->selectEnd = -1;
				tmpBlock->next->focus = tmpBlock->next->selectBegin = tmpBlock->next->selectEnd = 0;
				tmpBlock->next->content->focusPos =
					tmpBlock->next->content->selectBegin = tmpBlock->next->content->selectEnd =
					tmpBlock->next->content->content->length();
				Line *iterLine = tmpBlock->content;
				tmpBlock->size = 0;
				while (iterLine) {
					tmpBlock->size += iterLine->content->length();
					iterLine = iterLine->next;
				}
				tmpBlock = tmpBlock->next;
				tmpBlock->size = cont[i].length();
				tmpLine = tmpBlock->content;
				actFile->focus = actFile->selectBegin = actFile->selectEnd = actFile->focus + 1;
				actFile->total++;
			}
		}
		else {
			if (tmpLine->next) {
				tmpLine->next->prev = new Line(cont[i]);
				tmpLine->next->prev->next = tmpLine->next;
				tmpLine->next = tmpLine->next->prev;
				tmpLine->next->prev = tmpLine;
				tmpLine->focusPos = tmpLine->selectBegin = tmpLine->selectEnd = -1;
				tmpLine = tmpLine->next;
				tmpBlock->focus = tmpBlock->selectBegin = tmpBlock->selectEnd = tmpBlock->focus + 1;
				tmpBlock->total++;
				tmpBlock->size += cont[i].length();
			}
			else {
				tmpLine->next = new Line(cont[i]);
				tmpLine->next->prev = tmpLine;
				tmpLine->next->next = NULL;
				tmpLine = tmpLine->next;
				tmpBlock->focus = tmpBlock->selectBegin = tmpBlock->selectEnd = tmpBlock->focus + 1;
				tmpBlock->total++;
				tmpBlock->size += cont[i].length();
			}
		}
	}
	tmpLine->focusPos = tmpLine->selectBegin = tmpLine->selectEnd = tmpLine->content->length() - tmpFocus;
}
void Editor::choose() {
	File *actFile = this->actFile();

	Block *beginBlock = actFile->blockAt(actFile->selectBegin);
	Line *beginLine = beginBlock->lineAt(beginBlock->selectBegin);
	Block *endBlock = actFile->blockAt(actFile->selectEnd);
	Line *endLine = endBlock->lineAt(endBlock->selectEnd);
	Block *focusBlock = actFile->focusBlock();
	Line *focusLine = focusBlock->focusLine();
	beginLine->focusPos = beginLine->selectBegin = beginLine->selectEnd = -1;
	beginBlock->focus = beginBlock->selectBegin = beginBlock->selectEnd = -1;
	endLine->focusPos = endLine->selectBegin = endLine->selectEnd = -1;
	endBlock->focus = endBlock->selectBegin = endBlock->selectEnd = -1;
	focusLine->focusPos = focusLine->selectBegin = focusLine->selectEnd = -1;
	focusBlock->focus = focusBlock->selectBegin = focusBlock->selectEnd = -1;
	actFile->selectBegin = 0;
	actFile->focus = actFile->selectEnd = actFile->total - 1;
	beginBlock = actFile->content;
	endBlock = actFile->focusBlock();
	beginBlock->selectBegin = 0;
	endBlock->focus = endBlock->selectEnd = endBlock->total - 1;
	beginLine = beginBlock->content;
	endLine = endBlock->focusLine();
	beginLine->selectBegin = 0;
	endLine->focusPos = endLine->selectEnd = endLine->content->length();

}
void Editor::tab() {

}
void Editor::untab() {

}
void Editor::refresh() {

	//Main frame.
	setColor(127, 127, 127);
	putQuad(editorBase.x - 80, editorBase.y - 20, editorBase.x + editorSize.x - 1, editorBase.y - 1, SOLID_FILL);
	setColor(63, 63, 63);
	putQuad(fileListBase.x, fileListBase.y,
		fileListBase.x + fileListSize.x - 1, fileListBase.y + fileListSize.y - 1, SOLID_FILL);
	setColor(95, 95, 95);
	putQuad(fileListBase.x + 1, fileListBase.y + 1,
		fileListBase.x + fileListSize.x - 2, fileListBase.y + fileListSize.y - 2, EMPTY_FILL);
	setColor(255, 255, 255);
	setFontSize(20);
	setFontName((char *)"微软雅黑");
	putString((char *)"文件列表", 0, 0);
	setColor(31, 31, 31);
	putQuad(editorBase.x - 80, editorBase.y,
		editorBase.x + editorSize.x - 1, editorBase.y + editorSize.y - 1, SOLID_FILL);
	if (active == -1)return;

	//Globe variables.
	int x = editorBase.x, y = editorBase.y;
	File *actFile = this->actFile();
	Block *tmpBlock;
	Line *tmpLine;
	setFontSize(fontSize);
	setFontName((char *)fontName.c_str());

	//Cursor.
	if (actFile->focus == actFile->start) {
		tmpBlock = actFile->focusBlock();
		cursorPos.y = (tmpBlock->focus - tmpBlock->start) * (fontSize + 4);
	}
	else if (actFile->focus < actFile->start) {
		cursorPos.y = -(fontSize + 4);
	}
	else {
		int delta = 0;
		tmpBlock = actFile->startBlock();
		delta += tmpBlock->total - tmpBlock->start;
		tmpBlock = tmpBlock->next;
		for (int i = 0; i < actFile->focus - actFile->start - 1; i++) {
			delta += tmpBlock->total;
			tmpBlock = tmpBlock->next;
		}
		delta += tmpBlock->focus;
		cursorPos.y = delta * (fontSize + 4);
	}
	tmpLine = actFile->focusBlock()->focusLine();
	cursorPos.x = stringWidth(tmpLine->content->c_str(), tmpLine->focusPos);

	if (cursorPos.y >= 0 && cursorPos.y + fontSize + 4 < editorSize.y) {
		setColor(63, 63, 63);
		putQuad(editorBase.x - 2, editorBase.y + cursorPos.y,
			editorBase.x + editorSize.x, editorBase.y + cursorPos.y + fontSize + 2, EMPTY_FILL);
		putQuad(editorBase.x - 1, editorBase.y + cursorPos.y + 1,
			editorBase.x + editorSize.x - 1, editorBase.y + cursorPos.y + fontSize + 1, EMPTY_FILL);
	}

	//Selected background.
	int selectStartX, selectStartY;
	int selectEndX, selectEndY;

	tmpBlock = actFile->startBlock();
	if (actFile->selectBegin == actFile->start) {
		selectStartY = tmpBlock->selectBegin - tmpBlock->start;
		tmpLine = actFile->blockAt(actFile->selectBegin)->lineAt(tmpBlock->selectBegin);
		selectStartX = stringWidth(tmpLine->content->c_str(), tmpLine->selectBegin);
	}
	else if (actFile->selectBegin < actFile->start) {
		selectStartY = 0;
		selectStartX = 0;
	}
	else {
		int delta = 0;
		delta += tmpBlock->total - tmpBlock->start;
		tmpBlock = tmpBlock->next;
		for (int i = 0; i < actFile->selectBegin - actFile->start - 1; i++) {
			delta += tmpBlock->total;
			tmpBlock = tmpBlock->next;
		}
		delta += tmpBlock->selectBegin;
		selectStartY = delta;
		tmpLine = actFile->blockAt(actFile->selectBegin)->lineAt(tmpBlock->selectBegin);
		selectStartX = stringWidth(tmpLine->content->c_str(), tmpLine->selectBegin);
	}

	tmpBlock = actFile->startBlock();
	if (actFile->selectEnd == actFile->start) {
		selectEndY = tmpBlock->selectEnd - tmpBlock->start;
	}
	else if (actFile->selectEnd < actFile->start) {
		selectEndY = 0;
	}
	else {
		int delta = 0;
		delta += tmpBlock->total - tmpBlock->start;
		tmpBlock = tmpBlock->next;
		for (int i = 0; i < actFile->selectEnd - actFile->start - 1; i++) {
			delta += tmpBlock->total;
			tmpBlock = tmpBlock->next;
		}
		delta += tmpBlock->selectEnd;
		selectEndY = delta;
	}
	tmpLine = actFile->blockAt(actFile->selectEnd)->lineAt(tmpBlock->selectEnd);
	selectEndX = stringWidth(tmpLine->content->c_str(), tmpLine->selectEnd);
	
	if (selectStartY > selectEndY || (selectStartY == selectEndY && selectStartX > selectEndX)) {
		selectStartX ^= selectEndX ^= selectStartX ^= selectEndX;
		selectStartY ^= selectEndY ^= selectStartY ^= selectEndY;
	}
	setColor(selectColor.r, selectColor.g, selectColor.b);
	if (selectStartY < 0) {
		selectStartY = 0;
		selectStartX = 0;
	}
	if (selectStartY == selectEndY) {
		if(selectStartX < selectEndX)
			putQuad(editorBase.x + selectStartX, editorBase.y + selectStartY * (fontSize + 4),
				editorBase.x + selectEndX, editorBase.y + (selectStartY + 1) * (fontSize + 4) - 2, SOLID_FILL);
	}
	else if(selectEndY > 0)  {
		putQuad(editorBase.x + selectStartX, editorBase.y + selectStartY * (fontSize + 4),
			editorBase.x + editorSize.x, editorBase.y + (selectStartY + 1) * (fontSize + 4) - 2, SOLID_FILL);
		selectStartY++;
		while (selectStartY < selectEndY) {
			putQuad(editorBase.x, editorBase.y + selectStartY * (fontSize + 4),
				editorBase.x + editorSize.x, editorBase.y + (selectStartY + 1) * (fontSize + 4) - 2, SOLID_FILL);
			selectStartY++;
			if (selectStartY * (fontSize + 4) > editorSize.y)break;
		}
		putQuad(editorBase.x, editorBase.y + selectStartY * (fontSize + 4),
			editorBase.x + selectEndX, editorBase.y + (selectStartY + 1) * (fontSize + 4) - 2, SOLID_FILL);
	}

	//Edit area.
	tmpBlock = actFile->startBlock();

	Language *lex = NULL;
	if (actFile->format == "sgs") {
		lex = new SgsLex();
	}
	else if (actFile->format == "s") {
		lex = new MipsLex();
	}

	int ln = actFile->startLine() + 1;
	while (tmpBlock) {
		Line *tmpLine = tmpBlock->startLine();
		while (tmpLine) {
			if (lex) {
				lex->input(tmpLine->content->c_str());
				std::vector<std::pair<int, std::pair<int, int>>> hl = lex->highlight();

				if (actFile->format == "sgs") {
					string tmp;
					for (unsigned int i = 0; i < hl.size(); i++) {
						switch (hl[i].first) {
						case SGS_TT_USER:
							setColor(255, 255, 255);
							break;
						case SGS_TT_SYS:
							setColor(95, 159, 223);
							break;
						case SGS_TT_OP:
							setColor(0, 255, 255);
							break;
						case SGS_TT_DATA:
							setColor(223, 159, 127);
							break;
						default:
							setColor(255, 255, 255);
							break;
						}
						tmp = string(tmpLine->content->begin() + hl[i].second.first, tmpLine->content->begin() + hl[i].second.second);

						int px = x + stringWidth(tmpLine->content->c_str(), hl[i].second.first);
						int py = y;
						putString(tmp.c_str(), px, py);
					}

					if (!hl.empty() && hl[hl.size() - 1].second.second < (int)tmpLine->content->size()) {
						tmp = string(tmpLine->content->begin() + hl[hl.size() - 1].second.second, tmpLine->content->end());
						int px = x + stringWidth(tmpLine->content->c_str(), hl[hl.size() - 1].second.second);
						int py = y;
						setColor(95, 159, 63);
						putString(tmp.c_str(), px, py);
					}
					else if (hl.empty() && tmpLine->content->size() > 0) {
						setColor(95, 159, 63);
						putString(tmpLine->content->c_str(), x, y);
					}
				}
				else if(actFile->format == "s") {
					string tmp;
					for (unsigned int i = 0; i < hl.size(); i++) {
						switch (hl[i].first) {
						case MIPS_TT_USER:
							setColor(255, 255, 255);
							break;
						case MIPS_TT_SYS:
							setColor(95, 159, 223);
							break;
						case MIPS_TT_OP:
							setColor(0, 255, 255);
							break;
						case MIPS_TT_DATA:
							setColor(223, 159, 127);
							break;
						case MIPS_TT_REG:
							setColor(191, 191, 63);
							break;
						default:
							setColor(255, 255, 255);
							break;
						}
						tmp = string(tmpLine->content->begin() + hl[i].second.first, tmpLine->content->begin() + hl[i].second.second);

						int px = x + stringWidth(tmpLine->content->c_str(), hl[i].second.first);
						int py = y;
						putString(tmp.c_str(), px, py);
					}

					if (!hl.empty() && hl[hl.size() - 1].second.second < (int)tmpLine->content->size()) {
						tmp = string(tmpLine->content->begin() + hl[hl.size() - 1].second.second, tmpLine->content->end());
						int px = x + stringWidth(tmpLine->content->c_str(), hl[hl.size() - 1].second.second);
						int py = y;
						setColor(95, 159, 63);
						putString(tmp.c_str(), px, py);
					}
					else if (hl.empty() && tmpLine->content->size() > 0) {
						setColor(95, 159, 63);
						putString(tmpLine->content->c_str(), x, y);
					}
				}
			}
			else {
				setColor(255, 255, 255);
				putString(tmpLine->content->c_str(), x, y);
			}

			char num[16];
			setColor(lnColor.r, lnColor.g, lnColor.b);
			setFontSize(fontSize);
			setFontName(fontName.c_str());
			sprintf(num, "%d", ln++);
			int xoff = stringWidth(num, strlen(num));
			putString(num, editorBase.x - xoff - 22, y);

			y += fontSize + 4;
			if (y > editorSize.y + 20)break;
			tmpLine = tmpLine->next;
		}
		if (y > editorSize.y + 20)break;
		tmpBlock = tmpBlock->next;
	}
	delete lex;

	getWidgetByName("vert")->hide = actFile->totalLine();
	getWidgetByName("vert")->value = actFile->startLine();

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
		if (selecting()) {
			actFile->focus = actFile->selectBegin = actFile->selectEnd;
			Block *tmpBlock = actFile->focusBlock();
			tmpBlock ->focus = tmpBlock->selectBegin = tmpBlock->selectEnd;
			Line *tmpLine = tmpBlock->focusLine();
			tmpLine->focusPos = tmpLine->selectBegin = tmpLine->selectEnd;
		}
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
		if (selecting()) {
			actFile->focus = actFile->selectEnd = actFile->selectBegin;
			Block *tmpBlock = actFile->focusBlock();
			tmpBlock->focus = tmpBlock->selectEnd = tmpBlock->selectBegin;
			Line *tmpLine = tmpBlock->focusLine();
			tmpLine->focusPos = tmpLine->selectEnd = tmpLine->selectBegin;
		}
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
		if (selecting()) {
			actFile->focus = actFile->selectEnd = actFile->selectBegin;
			Block *tmpBlock = actFile->focusBlock();
			tmpBlock->focus = tmpBlock->selectEnd = tmpBlock->selectBegin;
			Line *tmpLine = tmpBlock->focusLine();
			tmpLine->focusPos = tmpLine->selectEnd = tmpLine->selectBegin;
		}
		else {
			Block *tmpBlock = actFile->focusBlock();
			Line *tmpLine = tmpBlock->focusLine();
			if (tmpLine->focusPos == 0) {
				return;
			}
			else {
				tmpLine->focusPos--;
			}
		}
		clearKeyBuffer();
	}
	if (key == SG_RIGHT) {
		if (selecting()) {
			actFile->focus = actFile->selectBegin = actFile->selectEnd;
			Block *tmpBlock = actFile->focusBlock();
			tmpBlock->focus = tmpBlock->selectBegin = tmpBlock->selectEnd;
			Line *tmpLine = tmpBlock->focusLine();
			tmpLine->focusPos = tmpLine->selectBegin = tmpLine->selectEnd;
		}
		else {
			Block *tmpBlock = actFile->focusBlock();
			Line *tmpLine = tmpBlock->focusLine();
			if (tmpLine->focusPos > (int)tmpLine->content->size() - 1) {
				return;
			}
			else {
				tmpLine->focusPos++;
			}
		}
		clearKeyBuffer();
	}
	if (key >= 0x20 && key < 0x80) {
		if (selecting())cut();
		Block *tmpBlock = actFile->focusBlock();
		MipsLex lex;
		vector<string> compList = lex.complete(tmpBlock->focusLine()->insert(key));
		//Popup pop;
		//pop.add(compList);
		//pop.prepare();
		//pop.show(editorBase.x + cursorPos.x, editorBase.y + cursorPos.y + fontSize);
		tmpBlock->size++;
	}
	if (key == '\b') {
		if (selecting())cut();
		else {
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

					tmpBlock->selectBegin--;
					tmpBlock->selectEnd--;
					tmpBlock->focus--;
					tmpBlock->total--;
				}
				else if (tmpBlock->prev) {
					if (tmpLine->next) {
						tmpBlock->content = tmpLine->next;
						tmpLine->next = NULL;
						delete tmpBlock->content->prev;
						tmpBlock->content->prev = NULL;
						tmpBlock->selectBegin = -1;
						tmpBlock->selectEnd = -1;
						tmpBlock->focus = -1;
						tmpBlock->total--;
						tmpBlock->size -= left.length();
						tmpBlock = tmpBlock->prev;
						tmpBlock->size += left.length();
						actFile->selectBegin--;
						actFile->selectEnd--;
						actFile->focus--;
						tmpBlock->focus = tmpBlock->total - 1;
						tmpBlock->selectBegin = tmpBlock->total - 1;
						tmpBlock->selectEnd = tmpBlock->total - 1;
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
						tmpBlock->size += left.length();

						actFile->selectBegin--;
						actFile->selectEnd--;
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
		}
		clearKeyBuffer();
	}
	if (key == '\r') {
		if (selecting())cut();
		Block *tmpBlock = actFile->focusBlock();
		Line *tmpLine = tmpBlock->focusLine();
		string more = tmpLine->cut();

		int tab = 0;
		while (tmpLine->content->operator[](tab++) == '\t') {
			more = '\t' + more;
		}
		tmpBlock->size += tab - 1;
		if (tmpBlock->size > BLOCK_SIZE) {
			if (tmpBlock->next) {
				tmpBlock->next->prev = new Block(more);
				tmpBlock->next->prev->content->focusPos = tab - 1;
				tmpBlock->next->prev->content->next = tmpBlock->focusLine()->next;
				tmpBlock->focusLine()->next = NULL;
				tmpBlock->next->prev->content->next->prev = tmpBlock->next->prev->content;
				tmpBlock->next->total = tmpBlock->total - tmpBlock->focus;
				tmpBlock->total = tmpBlock->focus + 1;
				tmpBlock->next->prev->next = tmpBlock->next;
				tmpBlock->next = tmpBlock->next->prev;
				tmpBlock->next->prev = tmpBlock;
				int newSize = 0;
				Line *iterLine = tmpBlock->next->content;
				while (iterLine) {
					newSize += iterLine->content->length();
				}
				tmpBlock->next->size = newSize;
				tmpBlock->size -= tmpBlock->next->size;
				actFile->focus = actFile->selectBegin = actFile->selectEnd = actFile->focus + 1;;
				actFile->total++;
			}
			else {
				tmpBlock->next = new Block(more);
				tmpBlock->next->content->focusPos = tab - 1;
				tmpBlock->next->prev = tmpBlock;
				tmpBlock->next->content->next = tmpBlock->focusLine()->next;
				tmpBlock->focusLine()->next = NULL;
				tmpBlock->next->content->next->prev = tmpBlock->next->content;
				tmpBlock->next->total = tmpBlock->total - tmpBlock->focus;
				tmpBlock->total = tmpBlock->focus + 1;
				int newSize = 0;
				Line *iterLine = tmpBlock->next->content;
				while (iterLine) {
					newSize += iterLine->content->length();
				}
				tmpBlock->next->size = newSize;
				tmpBlock->size -= tmpBlock->next->size;
				actFile->focus = actFile->selectBegin = actFile->selectEnd = actFile->focus + 1;;
				actFile->total++;
			}
		}
		else {
			if (tmpLine->next) {
				tmpLine->next->prev = new Line(more);
				tmpLine->next->prev->focusPos = tab - 1;
				tmpLine->next->prev->next = tmpLine->next;
				tmpLine->next = tmpLine->next->prev;
				tmpLine->next->prev = tmpLine;
				tmpBlock->focus = tmpBlock->selectBegin = tmpBlock->selectEnd = tmpBlock->focus + 1;;
				tmpBlock->total++;
			}
			else {
				tmpLine->next = new Line(more);
				tmpLine->next->focusPos = tab - 1;
				tmpLine->next->prev = tmpLine;
				tmpBlock->focus = tmpBlock->selectBegin = tmpBlock->selectEnd = tmpBlock->focus + 1;;
				tmpBlock->total++;
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
		if (selecting())tab();
		Block *tmpBlock = actFile->focusBlock();
		tmpBlock->focusLine()->insert('\t');
		tmpBlock->size++;
	}
	if (key == (SG_SHIFTBIT | '\t')) {
		if (selecting())untab();
	}
	
	refresh();
}
void Editor::mouse(vecThree mouse) {
	if (active == -1)return;

	File *actFile = this->actFile();

	if (mouse.m == (SG_LEFT_BUTTON | SG_BUTTON_UP)) {
		if (actFile->selectBegin > actFile->selectEnd) {
			actFile->selectBegin ^= actFile->selectEnd ^= actFile->selectBegin ^= actFile->selectEnd;
			Block *beginBlock = actFile->blockAt(actFile->selectBegin);
			beginBlock->selectBegin = beginBlock->selectEnd;
			Block *endBlock = actFile->blockAt(actFile->selectEnd);
			endBlock->selectEnd = endBlock->selectBegin;
			Line *beginLine = beginBlock->lineAt(beginBlock->selectBegin);
			beginLine->selectBegin = beginLine->selectEnd;
			Line *endLine = endBlock->lineAt(endBlock->selectEnd);
			endLine->selectEnd = endLine->selectBegin;
		}
		else if (actFile->selectBegin == actFile->selectEnd) {
			Block *tmpBlock = actFile->blockAt(actFile->selectBegin);
			if (tmpBlock->selectBegin > tmpBlock->selectEnd) {
				tmpBlock->selectBegin ^= tmpBlock->selectEnd ^= tmpBlock->selectBegin ^= tmpBlock->selectEnd;
				Line *beginLine = tmpBlock->lineAt(tmpBlock->selectBegin);
				beginLine->selectBegin = beginLine->selectEnd;
				Line *endLine = tmpBlock->lineAt(tmpBlock->selectEnd);
				endLine->selectEnd = endLine->selectBegin;
			}
			else if (tmpBlock->selectBegin == tmpBlock->selectEnd) {
				Line *tmpLine = tmpBlock->lineAt(tmpBlock->selectBegin);
				if (tmpLine->selectBegin > tmpLine->selectEnd)
					tmpLine->selectBegin ^= tmpLine->selectEnd ^= tmpLine->selectBegin ^= tmpLine->selectEnd;
			}
		}
	}
	if (INRECT(mouse.x, mouse.y, editorBase.x, editorBase.y,
		editorBase.x + editorSize.x - 20, editorBase.y + editorSize.y)) {
		if (mouse.m == (SG_LEFT_BUTTON | SG_BUTTON_DOWN)) {
			int emx = mouse.x - editorBase.x, emy = mouse.y - editorBase.y;
			if (emx < 0)emx = 0;
			if (emy < 0)emy = 0;
			editing = true;

			int line = emy / (fontSize + 4);
			actFile->selectBegin = actFile->selectEnd = actFile->focus = actFile->start;
			Block *tmpBlock = actFile->startBlock();
			if (tmpBlock->total - tmpBlock->start > line) {
				tmpBlock->selectBegin = tmpBlock->selectEnd = tmpBlock->focus = tmpBlock->start + line;

				setFontSize(fontSize);
				setFontName((char *)fontName.c_str());
				Line *tmpLine = tmpBlock->focusLine();
				int left = 0, right = 0;
				unsigned int i;
				for (i = 0; i < tmpLine->content->length(); i++) {
					if ((right = stringWidth((char *)tmpLine->content->c_str(), i)) <= emx)left = right;
					else break;
				}
				right = stringWidth((char *)tmpLine->content->c_str(), i);
				if (2 * emx < left + right)tmpLine->selectBegin = tmpLine->selectEnd = tmpLine->focusPos = i - 1;
				else tmpLine->selectBegin = tmpLine->selectEnd = tmpLine->focusPos = i;
			}
			else {
				line -= tmpBlock->total - tmpBlock->start;
				if (tmpBlock->next) {
					tmpBlock->selectBegin = tmpBlock->selectEnd = tmpBlock->focus = -1;
					tmpBlock = tmpBlock->next;
					actFile->focus = actFile->start + 1;
					actFile->selectBegin = actFile->start + 1;
					actFile->selectEnd = actFile->start + 1;
					while (line > tmpBlock->total) {
						if (tmpBlock->next) {
							line -= tmpBlock->total;
							tmpBlock->selectBegin = tmpBlock->selectEnd = tmpBlock->focus = -1;
							tmpBlock = tmpBlock->next;
							actFile->focus++;
							actFile->selectBegin++;
							actFile->selectEnd++;
						}
						else break;
					}
					if (line > tmpBlock->total)
						tmpBlock->selectBegin = tmpBlock->selectEnd = tmpBlock->focus = tmpBlock->total - 1;
					else
						tmpBlock->selectBegin = tmpBlock->selectEnd = tmpBlock->focus = line;
				}
				else {
					tmpBlock->selectBegin = tmpBlock->selectEnd = tmpBlock->focus = tmpBlock->total - 1;
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
				right = stringWidth((char *)tmpLine->content->c_str(), i);
				if (2 * emx < left + right)tmpLine->selectBegin = tmpLine->selectEnd = tmpLine->focusPos = i - 1;
				else tmpLine->selectBegin = tmpLine->selectEnd = tmpLine->focusPos = i;
			}
		}
		if (mouse.m & (SG_MIDDLE_BUTTON_DOWN | SG_MIDDLE_BUTTON_UP)) {
			if (mouse.m == SG_MIDDLE_BUTTON_DOWN)scroll(3);
			if(mouse.m == SG_MIDDLE_BUTTON_UP)scroll(-3);
		}
	}
	else {
		editing = false;
		if (INRECT(mouse.x, mouse.y, fileListBase.x, fileListBase.y,
			fileListBase.x + fileListSize.x, fileListBase.y + fileListSize.y)) {
			int line = (mouse.y - 20) / 20;
			if (line < fileNum)active = line;
		}
	}

	refresh();
}
void Editor::drag(vecTwo pos) {
	if (active == -1 || editing == false)return;

	File *actFile = this->actFile();

	if (editing) {
		if (pos.y < editorBase.y)scroll(-1);
		if (pos.y >= editorBase.y + editorSize.y)scroll(1);

		int emx = pos.x - editorBase.x, emy = pos.y - editorBase.y;
		if (emx < 0)emx = 0;
		if (emy < 0)emy = 0;

		int line = emy / (fontSize + 4);
		actFile->selectEnd = actFile->focus = actFile->start;
		Block *tmpBlock = actFile->startBlock();
		if (tmpBlock->total - tmpBlock->start > line) {
			tmpBlock->selectEnd = tmpBlock->focus = tmpBlock->start + line;

			setFontSize(fontSize);
			setFontName((char *)fontName.c_str());
			Line *tmpLine = tmpBlock->focusLine();
			int left = 0, right = 0;
			unsigned int i;
			for (i = 0; i < tmpLine->content->length(); i++) {
				if ((right = stringWidth((char *)tmpLine->content->c_str(), i)) <= emx)left = right;
				else break;
			}
			right = stringWidth((char *)tmpLine->content->c_str(), i);
			if (2 * emx < left + right)tmpLine->selectEnd = tmpLine->focusPos = i - 1;
			else tmpLine->selectEnd = tmpLine->focusPos = i;
		}
		else {
			line -= tmpBlock->total - tmpBlock->start;
			if (tmpBlock->next) {
				tmpBlock->selectEnd = tmpBlock->focus = -1;
				tmpBlock = tmpBlock->next;
				actFile->focus = actFile->start + 1;
				actFile->selectEnd = actFile->start + 1;
				while (line > tmpBlock->total) {
					if (tmpBlock->next) {
						line -= tmpBlock->total;
						tmpBlock->selectEnd = tmpBlock->focus = -1;
						tmpBlock = tmpBlock->next;
						actFile->focus++;
						actFile->selectEnd++;
					}
					else break;
				}
				if (line > tmpBlock->total)
					tmpBlock->selectEnd = tmpBlock->focus = tmpBlock->total - 1;
				else
					tmpBlock->selectEnd = tmpBlock->focus = line;
			}
			else {
				tmpBlock->selectEnd = tmpBlock->focus = tmpBlock->total - 1;
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
			right = stringWidth((char *)tmpLine->content->c_str(), i);
			if (2 * emx < left + right)tmpLine->selectEnd = tmpLine->focusPos = i - 1;
			else tmpLine->selectEnd = tmpLine->focusPos = i;
		}
	}

	refresh();
}
void Editor::scroll(int line) {
	File *actFile = this->actFile();
	if (line > 0) {
		Block *tmpBlock = actFile->startBlock();
		if (tmpBlock->start + line < tmpBlock->total) {
			tmpBlock->start += line;
		}
		else if(tmpBlock->next) {
			line -= tmpBlock->total - tmpBlock->start;
			while (tmpBlock->next) {
				tmpBlock->start = 0;
				actFile->start++;
				tmpBlock = tmpBlock->next;
				if (line < tmpBlock->total)break;
				line -= tmpBlock->total;
			}
			if(line < tmpBlock->total)
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
		else if(tmpBlock->prev) {
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
int Editor::getNum() {
	return fileNum;
}
