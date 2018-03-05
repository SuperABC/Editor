#include "line.h"

Line::Line(vector<string> &cont, int &count, int offset) : content(NULL), next(NULL) {
	count++;

	content = new string(cont[offset]);
	if (offset + 1 < cont.size())
		next = new Line(cont, count, offset + 1);
}
Line::~Line() {
	if (next)delete next;
	delete content;
}