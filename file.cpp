#include "file.h"

File::File() : filename("new file"), next(NULL), prev(NULL) {
	content = new Block();
	start = 0;
	total = 1;
	focus = 0;
}
File::File(char *filename) : filename(filename),
	content(NULL), next(NULL), prev(NULL) {
	string tmp;
	vector<string> whole;

	std::ifstream fin(filename);

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
