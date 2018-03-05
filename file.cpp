#include "file.h"

File::File(char *filename) : filename(filename), content(NULL), next(NULL) {
	string tmp;
	vector<string> whole;

	std::ifstream fin(filename);

	while (!fin.eof()) {
		getline(fin, tmp);
		whole.push_back(tmp);
	}

	content = new Block(whole, totalBlock);
	startBlock = 0;
	fin.close();
};
File::~File() {
	delete content;
}