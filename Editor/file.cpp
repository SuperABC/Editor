#include "file.h"

File::File() : filename("new file"), format(""), next(NULL), prev(NULL) {
	content = new Block();
	start = 0;
	total = 1;
	focus = 0;
}
File::File(char *filename) : filename(filename),
	content(NULL), next(NULL), prev(NULL) {
	string tmp;
	vector<string> whole;

	if (this->filename.rfind('.') == -1)format = string();
	else format = string(this->filename.begin() + this->filename.rfind('.') + 1, this->filename.end());

	std::ifstream fin(filename);

	if (!fin.is_open())return;

	while (!fin.eof()) {
		getline(fin, tmp);
		whole.push_back(tmp);
	}

	content = new Block(whole, total);
	start = 0;
	focus = 0;
	fin.close();
};
File::~File() {
	delete content;
}

string File::getPath() {
	return filename;
}
string File::getName() {
	int end = filename.rfind('/');
	if(end == -1)
		end = filename.rfind('\\');
	if (end == -1)return filename;
	else return string(filename.begin() + end + 1, filename.end());
}

Block *File::startBlock() {
	Block *tmpBlock = content;
	for (int i = 0; i < start; i++)tmpBlock = tmpBlock->next;
	return tmpBlock;
}
Block *File::focusBlock() {
	Block *tmpBlock = content;
	for (int i = 0; i < focus; i++)tmpBlock = tmpBlock->next;
	return tmpBlock;

}
