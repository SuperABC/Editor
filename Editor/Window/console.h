#pragma once
#include "Window/grid.h"

class Console :public Grid {
private:
	string content;

public:
	Console(int x, int y, int w, int h) : Grid("console", x, y, w, h), content("") {}

	virtual void moveGrid() {}
	virtual void showCont() {}
	virtual void keyEvent(int key) {}
	virtual void mouseEvent(int x, int y, int status) {}
	virtual void mouseMove(int x, int y) {}
	virtual void mouseDrag(int x, int y, int button) {}
	virtual void idelLoop() {}
};