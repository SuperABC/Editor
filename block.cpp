#include "block.h"

Block::Block(vector<string> &cont, int &count, int offset) : content(NULL), next(NULL) {
	int start = offset, end = offset;
	int length = 0;

	count++;
	while (length < BLOCK_SIZE / 2 && end < cont.size()) {
		length += cont[end++].length();
	}
	vector<string> line(cont.begin() + start, cont.begin() + end);
	content = new Line(line, totalLine);
	if(end < cont.size())
		next = new Block(cont, count, end);

	startLine = 0;
};
Block::~Block() {
	if (next)delete next;
	delete content;
}
