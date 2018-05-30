#pragma once
#include "Window/grid.h"
#include "Editor/editor.h"

class Prime :public Grid {
private:

public:
	Prime(int x, int y, int w, int h) : Grid("editor", x, y, w, h) {}

	virtual void moveGrid() {}
	virtual void showCont();
	virtual void keyEvent(int key) {}
	virtual void mouseEvent(int x, int y, int status) {}
	virtual void mouseMove(int x, int y) {}
	virtual void mouseDrag(int x, int y, int button) {}
	virtual void idelLoop() {}
};