#define _CRT_SECURE_NO_WARNINGS
#include "Editor/editor.h"
#include "Util/json.h"

extern Editor *editor;

Editor::Editor(string config) {
	content = NULL;
}
Editor::~Editor() {

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
				fout << tmpLine->content->data();
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
}
void Editor::insert(char c) {

}
void Editor::operate(int c) {

}
void Editor::cut() {
	File *actFile = this->actFile();
	if (actFile->selectBegin == actFile->selectEnd) {
		Block *tmpBlock = actFile->blockAt(actFile->selectBegin);
		if (tmpBlock->selectBegin == tmpBlock->selectEnd) {
			Line *tmpLine = tmpBlock->lineAt(tmpBlock->selectBegin);

			int preBlock = actFile->focus, preLine = tmpBlock->focus, preOffset = tmpLine->focusPos;
			vector<string> clips;
			string left = tmpLine->cut(tmpLine->selectEnd);
			clips.push_back(tmpLine->cut(tmpLine->selectBegin));
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

			int preBlock = actFile->focus, preLine = tmpBlock->selectEnd, preOffset = endLine->focusPos;
			vector<string> clips;

			Line *iterLine = beginLine->next;
			while (iterLine != endLine) {
				tmpBlock->size -= iterLine->content->length();
				iterLine = iterLine->next;
			}
			tmpBlock->size -= endLine->selectEnd;
			string left = endLine->cut(endLine->selectEnd);
			clips.push_back(beginLine->cut(beginLine->selectBegin));
			beginLine->append(left);
			Line *trashLine = beginLine->next;
			beginLine->next = endLine->next;
			endLine->next = NULL;
			iterLine = trashLine;
			while (iterLine) {
				clips.push_back(*iterLine->content);
				iterLine = iterLine->next;
			}
			delete trashLine;
			if (beginLine->next)beginLine->next->prev = beginLine;
			tmpBlock->focus = tmpBlock->selectEnd = tmpBlock->selectBegin;
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

			int preBlock = actFile->focus, preLine = endBlock->selectEnd, preOffset = endLine->focusPos;
			vector<string> clips;

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
			clips.push_back(beginLine->cut(beginLine->selectBegin));
			beginLine->append(left);
			Line *trashLine;
			trashLine = beginLine->next;
			beginLine->next = NULL;
			iterLine = trashLine;
			while (iterLine) {
				clips.push_back(*iterLine->content);
				iterLine = iterLine->next;
			}
			delete trashLine;
			beginBlock->total = beginBlock->focus + 1;
			trashLine = endBlock->content;
			endBlock->content = endLine->next;
			endLine->next = NULL;

			vector<string> tails;
			iterLine = trashLine;
			while (iterLine) {
				tails.push_back(*iterLine->content);
				iterLine = iterLine->next;
			}
			delete trashLine;
			endBlock->total -= endBlock->focus + 1;
			endBlock->focus = endBlock->selectBegin = endBlock->selectEnd = -1;
			if (beginBlock->next != endBlock) {
				Block *trashBlock = beginBlock->next;
				endBlock->prev->next = NULL;
				Block *iterBlock = trashBlock;
				while (iterBlock) {
					iterLine = iterBlock->content;
					while (iterLine) {
						clips.push_back(*iterLine->content);
						iterLine = iterLine->next;
					}
					iterBlock = iterBlock->next;
				}
				delete trashBlock;
				beginBlock->next = endBlock;
				endBlock->prev = beginBlock;
			}
			for (auto s : tails)clips.push_back(s);
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

			int preBlock = actFile->focus, preLine = endBlock->selectEnd, preOffset = endLine->focusPos;
			vector<string> clips;

			Line *iterLine;
			beginBlock->size -= beginLine->content->length() - beginLine->selectBegin;
			beginBlock->size += endLine->content->length() - endLine->selectEnd;
			iterLine = beginLine->next;
			while (iterLine) {
				beginBlock->size -= iterLine->content->length();
				iterLine = iterLine->next;
			}
			string left = endLine->cut(endLine->selectEnd);
			clips.push_back(beginLine->cut(beginLine->selectBegin));
			beginLine->append(left);
			Line *trashLine;
			trashLine = beginLine->next;
			beginLine->next = NULL;
			iterLine = trashLine;
			while (iterLine) {
				clips.push_back(*iterLine->content);
				iterLine = iterLine->next;
			}
			delete trashLine;
			beginBlock->total = beginBlock->focus + 1;
			Block *trashBlock = beginBlock->next;
			beginBlock->next = endBlock->next;
			endBlock->next = NULL;
			Block *iterBlock = trashBlock;
			while (iterBlock) {
				iterLine = iterBlock->content;
				while (iterLine) {
					clips.push_back(*iterLine->content);
					iterLine = iterLine->next;
				}
				iterBlock = iterBlock->next;
			}
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
			if (tmpLine->selectBegin == tmpLine->selectEnd)return vector<string>();
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
	int preBlock = actFile->focus, preLine = tmpBlock->focus, preOffset = tmpLine->focusPos;
	int tmpFocus = tmpLine->content->length() - tmpLine->focusPos;
	vector<string>clip = cont;
	cont[cont.size() - 1].append(tmpLine->cut());
	tmpLine->append(cont[0]);
	for (int i = 1; i < (int)cont.size(); i++) {
		if (tmpBlock->size > BLOCK_SIZE) { // miss size change
			if (tmpBlock->next) {
				tmpBlock->next->prev = new Block(cont[i]);
				tmpBlock->next->prev->content->next = tmpLine->next;
				if(tmpLine->next)
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
void Editor::pre() {
	File *actFile = this->actFile();
	Block *tmpBlock = actFile->focusBlock();
	Line *tmpLine = tmpBlock->focusLine();

	string* cont = tmpLine->content;
	if (tmpLine->focusPos > 0)tmpLine->focusPos--;
	while (tmpLine->focusPos > 0 && (cont->operator[](tmpLine->focusPos) == ' ' ||
		cont->operator[](tmpLine->focusPos) == '\t'))
		tmpLine->focusPos--;
	if (tmpLine->focusPos == 0) {
		if (actFile->focus != 0 || actFile->content->focus != 0) {
			operate(SG_UP);
			end();
		}
		return;
	}
	while (cont->operator[](tmpLine->focusPos) != ' ' &&
		cont->operator[](tmpLine->focusPos) != '\t') {
		tmpLine->focusPos--;
		if (tmpLine->focusPos == -1) {
			tmpLine->focusPos = 0;
			return;
		}
	}
	tmpLine->focusPos++;
}
void Editor::post() {
	File *actFile = this->actFile();
	Block *tmpBlock = actFile->focusBlock();
	Line *tmpLine = tmpBlock->focusLine();

	string* cont = tmpLine->content;
	while (tmpLine->focusPos < (int)tmpLine->content->length() &&
		(cont->operator[](tmpLine->focusPos) == ' ' ||
		cont->operator[](tmpLine->focusPos) == '\t'))
		tmpLine->focusPos++;
	if (tmpLine->focusPos == (int)tmpLine->content->length()) {
		if (actFile->focus != actFile->total - 1 || actFile->focusBlock()->focus != actFile->focusBlock()->total - 1) {
			operate(SG_DOWN);
			home();
		}
		return;
	}
	while (cont->operator[](tmpLine->focusPos) != ' ' &&
		cont->operator[](tmpLine->focusPos) != '\t') {
		tmpLine->focusPos++;
		if (tmpLine->focusPos == (int)tmpLine->content->length()) {
			return;
		}
	}
}
void Editor::home() {
	File *actFile = this->actFile();
	Block *tmpBlock = actFile->focusBlock();
	Line *tmpLine = tmpBlock->focusLine();
	tmpLine->focusPos = 0;
}
void Editor::end() {
	File *actFile = this->actFile();
	Block *tmpBlock = actFile->focusBlock();
	Line *tmpLine = tmpBlock->focusLine();
	tmpLine->focusPos = (int)tmpLine->content->length();
}
void Editor::lineup() {
	home();
	insert(SG_ENTER);
	operate(SG_UP);
}
void Editor::linedown() {
	end();
	insert(SG_ENTER);
}
void Editor::start() {
	File *actFile = this->actFile();
	Block *tmpBlock = actFile->focusBlock();
	Line *tmpLine = tmpBlock->focusLine();
	tmpLine->focusPos = 0;
	tmpBlock->focus = -1;
	actFile->focus = 0;
	actFile->content->focus = 0;
	actFile->content->content->focusPos = 0;
}
void Editor::eof() {
	File *actFile = this->actFile();
	Block *tmpBlock = actFile->focusBlock();
	Line *tmpLine = tmpBlock->focusLine();
	tmpLine->focusPos = 0;
	tmpBlock->focus = -1;
	actFile->focus = actFile->total - 1;
	tmpBlock = actFile->focusBlock();
	tmpBlock->focus = tmpBlock->total - 1;
	tmpLine = tmpBlock->focusLine();
	tmpLine->focusPos = tmpLine->content->length();
}
void Editor::round() {
	File *actFile = this->actFile();
	Block *tmpBlock = actFile->focusBlock();
	Line *tmpLine = tmpBlock->focusLine();
	tmpLine->selectBegin = tmpLine->selectEnd = tmpLine->focusPos;

	int type = 0;
	char tmp;
	if (tmpLine->content->length() == 0)return;
	if(tmpLine->selectBegin < (int)tmpLine->content->length())
		tmp = (*tmpLine->content)[tmpLine->selectBegin];
	else tmp = (*tmpLine->content)[--tmpLine->selectBegin];
	if (tmp == ' ' || tmp == '\t')
		type = 0;
	else if ((tmp >= 'a' && tmp <= 'z') ||
		(tmp >= 'A' && tmp <= 'Z') ||
		(tmp >= '0' && tmp <= '9'))
		type = 1;
	else tmp = 2;
	
	while (tmpLine->selectBegin >= 0) {
		int ntype = 0;
		char ntmp = (*tmpLine->content)[tmpLine->selectBegin];
		if (ntmp == ' ' || ntmp == '\t')
			ntype = 0;
		else if ((ntmp >= 'a' && ntmp <= 'z') ||
			(ntmp >= 'A' && ntmp <= 'Z') ||
			(ntmp >= '0' && ntmp <= '9'))
			ntype = 1;
		else ntmp = 2;
		if (ntype != type) {
			break;
		}
		tmpLine->selectBegin--;
	}
	tmpLine->selectBegin++;

	while (tmpLine->selectEnd < (int)tmpLine->content->length()) {
		int ntype = 0;
		char ntmp = (*tmpLine->content)[tmpLine->selectEnd];
		if (ntmp == ' ' || ntmp == '\t')
			ntype = 0;
		else if ((ntmp >= 'a' && ntmp <= 'z') ||
			(ntmp >= 'A' && ntmp <= 'Z') ||
			(ntmp >= '0' && ntmp <= '9'))
			ntype = 1;
		else ntmp = 2;
		if (ntype != type) {
			break;
		}
		tmpLine->selectEnd++;
	}
}
void Editor::through() {
	File *actFile = this->actFile();
	Block *tmpBlock = actFile->focusBlock();
	Line *tmpLine = tmpBlock->focusLine();
	tmpLine->selectBegin = 0;
	tmpLine->selectEnd = tmpLine->content->length();
}

File *Editor::actFile() {
	File *actFile = content;
	for (int i = 0; i < active; i++)actFile = actFile->next;
	return actFile;
}
int Editor::getNum() {
	return fileNum;
}
