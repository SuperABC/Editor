#pragma once
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include "winsgl.h"

using std::string;
using std::vector;

class Grid {
private:
	string type;
	vecTwo base, size;

public:
	Grid(string t, int x, int y, int w, int h) : type(t) {
		base.x = x;
		base.y = y;
		size.x = w;
		size.y = h;
	}

	bool inGrid(int x, int y) { return INRECT(x, y, base.x, base.y, base.x + size.x, base.y + size.y); }

	virtual void moveGrid() = 0;
	virtual void showCont() = 0;
	virtual void keyEvent(int key) = 0;
	virtual void mouseEvent(int x, int y, int status) = 0;
	virtual void mouseMove(int x, int y) = 0;
	virtual void mouseDrag(int x, int y, int button) = 0;
	virtual void idelLoop() = 0;
};